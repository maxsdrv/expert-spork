package firmware

import (
	"archive/tar"
	"context"
	"crypto/sha256"
	"encoding/hex"
	"fmt"
	"io"
	"os"
	"path"
	"path/filepath"

	"aead.dev/minisign"
	"filippo.io/age"
	"github.com/opticoder/ctx-log/go/ctx_log"
	"github.com/ulikunitz/xz"
)

var logging = ctx_log.GetLogger(nil)

var identity age.Identity
var publicKey minisign.PublicKey

func Init(ageKey, minisignKey string) error {
	var err error

	identity, err = age.ParseX25519Identity(ageKey)
	if err != nil {
		return err
	}

	return publicKey.UnmarshalText([]byte(minisignKey))
}

func Extract(ctx context.Context, firmware string, dir string) error {
	logger := logging.WithCtxFields(ctx)

	entries, err := os.ReadDir(dir)
	if err != nil {
		return fmt.Errorf("failed to read dir: %v", err)
	}
	for _, entry := range entries {
		err = os.RemoveAll(path.Join(dir, entry.Name()))
		if err != nil {
			return fmt.Errorf("failed to remove file: %v", err)
		}
	}

	logger.Debugf("Opening file: %s", firmware)
	src, err := os.Open(firmware)
	if err != nil {
		return fmt.Errorf("failed to open file: %v", err)
	}
	info, err := src.Stat()
	if err != nil {
		return fmt.Errorf("failed to obtain file size: %v", err)
	}
	logger.Infof("The firmware file size: %d", info.Size())

	decrypted, err := age.Decrypt(src, identity)
	if err != nil {
		return fmt.Errorf("failed to decrypt file: %v", err)
	}

	decompressed, err := xz.NewReader(decrypted)
	if err != nil {
		return fmt.Errorf("failed to decompress file: %v", err)
	}

	arch := tar.NewReader(decompressed)

	var signature []byte
	sha := sha256.New()
	for {
		hdr, err := arch.Next()
		if err == io.EOF {
			break
		}
		if err != nil {
			return err
		}

		if hdr.Name == "signature" {
			logger.Debugf("Signature size: %d", hdr.Size)
			signature, err = io.ReadAll(arch)
			if err != nil {
				return err
			}
			continue
		}

		logger.Debugf("Creating file: %s", hdr.Name)
		file, err := os.Create(filepath.Join(dir, hdr.Name))
		if err != nil {
			return err
		}

		logger.Debugf("Writing file: %s, size: %d", hdr.Name, hdr.Size)
		tee := io.MultiWriter(file, sha)
		size, err := io.Copy(tee, arch)
		if err != nil {
			return err
		}

		if err := file.Close(); err != nil {
			return err
		}
		logger.Infof("Written file: %s, size: %d", hdr.Name, size)
	}

	if signature == nil {
		return fmt.Errorf("signature not found")
	}
	logger.Debugf("Files sha: %s", hex.EncodeToString(sha.Sum(nil)))
	if !minisign.Verify(publicKey, sha.Sum(nil), signature) {
		logger.Debugf("signature: %s", string(signature))
		return fmt.Errorf("signature verification failed")
	}
	logger.Info("Signature is valid")
	if err := src.Close(); err != nil {
		logger.WithError(err).Warnf("failed to close file")
	}
	return nil
}

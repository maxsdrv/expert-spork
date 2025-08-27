package logs

import (
	"context"
	"io"
)

// ORIGIN: https://stackoverflow.com/a/63794069
type readerCtx struct {
	ctx      context.Context
	reader   io.Reader
	size     int64
	count    int
	progress int
}

func (r *readerCtx) Read(p []byte) (int, error) {
	if r.ctx.Err() != nil {
		return 0, r.ctx.Err()
	}
	size, err := r.reader.Read(p)
	r.count += size
	progress := r.count / (10 * 1024 * 1024)
	if progress > r.progress {
		r.progress = progress
		logging.WithCtxFields(r.ctx).Debugf("Copied %d/%d bytes", r.count, r.size)
	}
	return size, err
}

// newReader gets a context-aware io.Reader
func newReader(ctx context.Context, reader io.Reader, name string, size int64) io.Reader {
	return &readerCtx{ctx: logging.SetCtxField(ctx, "file", name), reader: reader, size: size, count: 0, progress: 0}
}

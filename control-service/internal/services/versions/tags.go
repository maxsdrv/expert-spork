package versions

import (
	"context"
	"encoding/json"
	"fmt"
	"io"
	"net/http"
)

type NetworkConnectionError struct {
	Status string
}

func (e *NetworkConnectionError) Error() string {
	return fmt.Sprintf("versions unavailable: %s", e.Status)
}

func (s *Service) getImageTags(ctx context.Context, repo string) ([]string, error) {
	logger := logging.WithCtxFields(ctx)

	logger.Debugf("Getting tags for repo %s", repo)

	registryUrl := fmt.Sprintf("https://%s/v2/%s/%s/tags/list", s.RegistryHost, s.RegistryPath, repo)

	var token string
	if s.RegistryAuth != "" {
		var err error
		token, err = s.getToken(ctx, registryUrl)
		if err != nil {
			logger.Errorf("Failed to get JWT token: %s", err)
			return nil, err
		}
	}

	return s.fetchTags(ctx, registryUrl, token)
}

func (s *Service) fetchTags(ctx context.Context, registryUrl, token string) ([]string, error) {
	var authHeader string
	if token != "" {
		authHeader = "Bearer " + token
	}
	resp, err := doRequest(ctx, registryUrl, authHeader)
	if err != nil {
		return nil, err
	}
	defer resp.Body.Close()

	if resp.StatusCode == http.StatusOK {
		var result struct {
			Tags []string
		}
		if err := json.NewDecoder(resp.Body).Decode(&result); err != nil {
			return nil, err
		}
		return result.Tags, nil
	} else {
		bodyBytes, _ := io.ReadAll(resp.Body)
		return nil, fmt.Errorf("unexpected status %d: %s", resp.StatusCode, string(bodyBytes))
	}
}

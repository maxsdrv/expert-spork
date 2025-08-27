package versions

import (
	"context"
	"encoding/json"
	"fmt"
	"net/http"
	"net/url"
	"strings"
	"time"
)

func (s *Service) parseDockerConfig(ctx context.Context, configJson string) (registryAuth string, err error) {
	logger := logging.WithCtxFields(ctx)

	logger.Debugf("Parsing docker config: %s", configJson)

	type authEntry struct {
		Auth string `json:"auth"`
	}
	type dockerConfigEntry struct {
		Auths map[string]authEntry `json:"auths"`
	}

	var cfg dockerConfigEntry
	if err := json.Unmarshal([]byte(configJson), &cfg); err != nil {
		logger.WithError(err).Errorf("Failed to parse docker config: %s", configJson)
		return "", err
	}

	if entry, ok := cfg.Auths[s.RegistryHost]; ok {
		return entry.Auth, nil
	}

	return "", nil
}

func (s *Service) getToken(ctx context.Context, registryUrl string) (string, error) {
	logger := logging.WithCtxFields(ctx)

	logger.Debugf("Getting JWT token")

	tokenUrl, err := getTokenRequestUrl(ctx, registryUrl)
	if err != nil {
		logger.Errorf("Failed to get token url: %s", err)
		return "", err
	}

	authHeader := "Basic " + s.RegistryAuth
	resp, err := doRequest(ctx, tokenUrl, authHeader)
	if err != nil {
		logger.Errorf("Failed to make request: %v", err)
		return "", err
	}
	defer resp.Body.Close()

	if resp.StatusCode != http.StatusOK {
		logger.Errorf("Failed status JWT token")
		return "", fmt.Errorf(resp.Status)
	}

	var tokenResponse struct {
		Token string `json:"token"`
	}
	if err := json.NewDecoder(resp.Body).Decode(&tokenResponse); err != nil {
		logger.Errorf("Failed to decode JWT token")
		return "", err
	}

	return tokenResponse.Token, nil
}

func getTokenRequestUrl(ctx context.Context, registryUrl string) (string, error) {
	logger := logging.WithCtxFields(ctx)

	logger.Debugf("Getting token url")

	resp, err := doRequest(ctx, registryUrl, "")
	if err != nil {
		return "", err
	}
	defer resp.Body.Close()

	if resp.StatusCode != http.StatusUnauthorized {
		return "", fmt.Errorf("expected 401 Unauthorized, got %d", resp.StatusCode)
	}

	authHeader := resp.Header.Get("WWW-Authenticate")
	if authHeader == "" {
		return "", fmt.Errorf("WWW-Authenticate header not found")
	}

	const prefix = "Bearer "
	if !strings.HasPrefix(authHeader, prefix) {
		return "", fmt.Errorf("unexpected header format: %s", authHeader)
	}
	authHeader = strings.TrimPrefix(authHeader, prefix)
	parts := strings.Split(authHeader, ",")
	var params = map[string]string{}
	for _, part := range parts {
		part = strings.TrimSpace(part)
		kv := strings.SplitN(part, "=", 2)
		if len(kv) != 2 {
			return "", fmt.Errorf("malformed auth parameter: %q", part)
		}
		key := kv[0]
		value := strings.Trim(kv[1], `"`)
		params[key] = value
	}

	realm, ok := params["realm"]
	if !ok || realm == "" {
		return "", fmt.Errorf("missing 'realm' parameter in auth header: %s", authHeader)
	}
	delete(params, "realm")

	tokenUrl, err := url.Parse(realm)
	if err != nil {
		logger.Errorf("Failed to parse token URL: %s", realm)
		return "", err
	}

	queryParams := tokenUrl.Query()
	for key, value := range params {
		queryParams.Set(key, value)
	}
	tokenUrl.RawQuery = queryParams.Encode()

	return tokenUrl.String(), nil
}

func doRequest(ctx context.Context, url, authHeader string) (*http.Response, error) {
	client := &http.Client{Timeout: time.Second * 10}
	req, err := http.NewRequestWithContext(ctx, http.MethodGet, url, nil)
	if err != nil {
		return nil, err
	}

	if authHeader != "" {
		req.Header.Set("Authorization", authHeader)
	}

	resp, err := client.Do(req)
	if err != nil {
		return nil, &NetworkConnectionError{Status: err.Error()}
	}

	return resp, nil
}

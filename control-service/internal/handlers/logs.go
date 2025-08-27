package handlers

import (
	"errors"
	"io"
	"net/http"
	"net/url"
	"strings"

	"dds-control-api/internal/generated/api"
)

func (s *Handlers) GetLogs(w http.ResponseWriter, r *http.Request) {
	logger := logging.WithCtxFields(r.Context())

	responseWithError := func(err error, httpCode int) {
		api.DefaultErrorHandler(w, r, err, &api.ImplResponse{Code: httpCode})
	}

	query, err := url.ParseQuery(r.URL.RawQuery)
	if err != nil {
		logger.Error("GetLogs params decode error")
		responseWithError(err, http.StatusBadRequest)
		return
	}

	if !query.Has("logs_list") {
		logger.Error("Missing logs_list parameter")
		responseWithError(errors.New("missing logs_list parameter"), http.StatusBadRequest)
		return
	}
	logsList := strings.TrimSpace(query.Get("logs_list"))
	if len(logsList) == 0 {
		logger.Error("Empty logs_list parameter")
		responseWithError(errors.New("empty logs_list parameter"), http.StatusBadRequest)
		return
	}

	logger.Debug("Calling GetLogs controller")
	response, err := s.controllers.GetLogs(r.Context(), logsList)
	if err != nil {
		logger.WithError(err).Error("GetLogs call error")
		responseWithError(err, http.StatusInternalServerError)
		return
	}
	reader, ok := response.Body.(io.Reader)
	if !ok {
		logger.Error("GetLogs response type error")
		responseWithError(errors.New("internal error"), http.StatusInternalServerError)
		return
	}

	headers := w.Header()
	headers.Set("Content-Type", "application/octet-stream")
	headers.Set("Content-Disposition", "attachment; filename=logs.tar")
	w.WriteHeader(http.StatusOK)

	logger.Infof("Streaming response started")
	size, err := io.Copy(w, reader)
	if err != nil {
		logger.WithError(err).Errorf("Failed writing response data")
		return
	}

	logger.Infof("Finished streaming response, data size: %d", size)
}

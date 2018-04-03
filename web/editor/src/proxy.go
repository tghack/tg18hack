package main

import (
	"encoding/hex"
	"io"
	"net/http"
	"strings"
)

type HttpMux struct {
	w      http.ResponseWriter
	r      *http.Request
	public string
}

func TrimHex(r rune) bool {
	return ((r < '0') || (r > '9')) && ((r < 'a') || (r > 'f')) && ((r < 'A') || (r > 'F'))
}

func (mux HttpMux) ServeHTTP(w http.ResponseWriter, r *http.Request) {
	var url string

	trimmedUrl := strings.ToLower(strings.Trim(r.URL.Path, "/"))

	if strings.HasPrefix(strings.ToLower(trimmedUrl), "proxy") {
		tmp, err := hex.DecodeString(strings.TrimFunc(strings.TrimPrefix(trimmedUrl, "proxy"), TrimHex))

		if err != nil {
			w.WriteHeader(http.StatusForbidden)
			w.Write([]byte("Invalid hex URL"))
			return
		}
		url = string(tmp)
	} else {
		url = mux.public + strings.TrimLeft(r.URL.Path, "/")
	}
	resp, err := http.Get(url)

	if err != nil {
		w.WriteHeader(http.StatusForbidden)
		w.Write([]byte("Connection failed!"))
		return
	}
	w.Header().Set("Content-Type", resp.Header.Get("Content-Type"))
	io.Copy(w, resp.Body)
}

package main

import (
	"fmt"
	"net/http"
	"os"
	"strconv"
	"strings"
	"sync"
)

var wg sync.WaitGroup
const primaryPort int64 = 2200

type FileServeMux struct {
	baseFolder string
}

func (mux FileServeMux) ServeHTTP(w http.ResponseWriter, r *http.Request) {
	trimmed := strings.Trim(r.URL.Path, "/")

	if len(trimmed) > 0 {
		trimmed = "/" + trimmed
	}
	http.ServeFile(w, r, mux.baseFolder+trimmed)
}

func main() {

	if len(os.Args) != 2 {
		fmt.Printf("%s <folder>\n", os.Args[0])
		return
	}

	proxyMux := &HttpMux{}
	privateMux := &FileServeMux{}
	publicMux := &FileServeMux{}

	trimmed := strings.TrimRight(os.Args[1], "/")

	var base string

	if len(trimmed) > 0 {
		base = trimmed + "/"
	}

	privateMux.baseFolder = base + "private"
	publicMux.baseFolder = base + "public"

	proxyPort := strconv.FormatInt(primaryPort, 10)
	privatePort := strconv.FormatInt(primaryPort+1, 10)
	publicPort := strconv.FormatInt(primaryPort+2, 10)

	proxyMux.public = "http://127.0.0.1:" + publicPort + "/"

	wg.Add(3)
    go listen(":"+proxyPort, proxyMux)
	go listen("127.0.0.1:"+privatePort, privateMux)
	go listen("127.0.0.1:"+publicPort, publicMux)
	wg.Wait()
}

func listen(listener string, handler http.Handler) {
	err := http.ListenAndServe(listener, handler)
	if err != nil {
		fmt.Println(err)
	}
	wg.Done()
}

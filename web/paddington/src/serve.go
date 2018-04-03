package main

import (
    "fmt"
    "net/http"
    "os"
    "strings"
    "crypto/hmac"
    "crypto/sha256"
    "encoding/hex"
    "time"
    "strconv"
)

var secret = "6707b46d4826bdbf"
type Mux struct {}

func newHash(str string)string{
    h := sha256.New()
    h.Write([]byte(secret))
    h.Write([]byte(str))
    return hex.EncodeToString(h.Sum(nil))
}

func (mux Mux) ServeHTTP(w http.ResponseWriter, r *http.Request) {
    w.Header().Set("Content-Type", "text/html; charset=utf-8")
    var cookieValue string
    for _,v := range r.Cookies(){
        if strings.EqualFold(v.Name, "user-auth"){
            cookieValue = v.Value
            break
        }
    }

    cookieSplit := strings.Split(cookieValue, "-")
    var valid bool
    if len(cookieSplit) == 2{
        valid = validate(cookieSplit[0], cookieSplit[1])
    }
    if valid{
        w.WriteHeader(http.StatusOK)
        w.Write(writeHtml("TG18{no_one_told_me_that_sha256_was_insecure}"))
    }else{
        w.WriteHeader(http.StatusUnauthorized)
        w.Write(writeHtml("403 Forbidden"))
    }
}

func writeHtml(txt string)[]byte{
    return []byte(fmt.Sprintf("<html><body><div>%s</div></body></html>", txt))
}

func isSplitRune(r rune)bool{
    return r < '0'
}

func validate(queryString string, hash string)(valid bool){
    tmpQuery,err := hex.DecodeString(queryString)
    if err != nil{
        return false
    }

    userHash := []byte(newHash(string(tmpQuery)))
    testAccess := false
    if hmac.Equal([]byte(hash), userHash){
        testAccess = true
    }

    //Remove invalid ascii
    for k,v := range tmpQuery{
        if v > 0x7e{
            tmpQuery[k] = 0
        }
    }

    currentTime := time.Now().Unix()

    decodeQuery := string(tmpQuery)
    splitEntries := strings.FieldsFunc(decodeQuery, isSplitRune)
    for _,v := range splitEntries{
        tmp := strings.Split(v, "=")
        if len(tmp) != 2{
            continue
        }

        if (tmp[0] == "expiry") && testAccess{
            expiry, err := strconv.ParseInt(tmp[1], 10, 64)
            if (expiry > currentTime) && (err == nil){
                valid = true
            }
        }
    }
    return
}

func main() {
    if len(os.Args) != 2 {
        fmt.Printf("%s <listen>\n", os.Args[0])
        return
    }

    listener := os.Args[1]
    mux := &Mux{}

    err := http.ListenAndServe(listener, mux)
    if err != nil{
        fmt.Println(err)
    }
}


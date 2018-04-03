document.addEventListener("DOMContentLoaded", function(){
    document.getElementById("login").addEventListener("click", function(){
        var user = document.getElementById("user").value;
        var pass = document.getElementById("pass").value;
        var data = {user: user, pass: pass};

        var xhr = new XMLHttpRequest();
        xhr.responseType = "text";
        xhr.onreadystatechange = function () {
            if((xhr.readyState === XMLHttpRequest.DONE) && (xhr.status >= 100) && (xhr.status < 400)) {
                document.write(xhr.response);
            }
        };
        xhr.open("POST", '/login', true);
        xhr.setRequestHeader("Content-type", "application/json");
        xhr.send(JSON.stringify(data));
    });
});

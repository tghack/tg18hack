const base_url = "http://localhost:7342/api"; //"http://noob3-4.tghack.no/api";

function onProfileLoad() {
    if (!sessionStorage) {
        console.log("Oh noes! Your browser does not support sessionstorage..")
    }

    var storedName = sessionStorage.getItem("username");
    var storedOS = sessionStorage.getItem("os") || "Not inserted yet";
    var storedTextEditor = sessionStorage.getItem("text_editor") || "Not inserted yet";
    document.getElementById("username").innerHTML = '<span id="username">' + storedName + '</span>';
    document.getElementById("info_os").innerHTML = '<span id="info_os">' + storedOS + '</span>';
    document.getElementById("info_editor").innerHTML = '<span id="info_editor">' + storedTextEditor + '</span>';
}

function updateProfile() {
    const url = base_url + "/profile";
    var operatingSystem = document.getElementById("operating-system").value;
    var textEditor = document.getElementById("text-editor").value;

    var data = JSON.stringify({
        os: operatingSystem,
        text_editor: textEditor
    });

    var xhr = new XMLHttpRequest();
    xhr.open("PATCH", url);
    xhr.setRequestHeader("Content-type", "application/json");
    xhr.setRequestHeader('X-Access-Token', sessionStorage.getItem("jwt_token"));
    xhr.onreadystatechange = function () {
        if (xhr.readyState == 4 && xhr.status == 200) {
            var result = JSON.parse(xhr.responseText);
            if (result.flag) {
                document.getElementById('msg').innerHTML = '<span id="msg">' + result.error + '</span>';
            }
            window.sessionStorage.setItem("os", operatingSystem);
            window.sessionStorage.setItem("text_editor", textEditor);
            document.getElementById("info_os").innerHTML = '<span id="info_os">' + operatingSystem + '</span>';
            document.getElementById("info_editor").innerHTML = '<span id="info_editor">' + textEditor + '</span>';
        }
        if (xhr.readyState == 4 && xhr.status == 401) {
            var result = JSON.parse(xhr.responseText);
            document.getElementById('msg').innerHTML = '<span id="msg">' + result.error + '</span>';
        }
    }
    xhr.send(data);
}

function logout() {
    window.sessionStorage.removeItem("jwt_token");
    window.sessionStorage.removeItem("username");
    window.sessionStorage.removeItem("os");
    window.sessionStorage.removeItem("text_editor");
    window.location.href = "http://localhost:7341/"; // "http://noob3-4.tghack.no/";

}
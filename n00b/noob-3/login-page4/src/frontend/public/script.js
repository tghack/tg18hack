const base_url = "http://localhost:7342/api"; //"http://noob3-4.tghack.no/api";

function onLoginPageLoad() {
	var storedName = sessionStorage.getItem("jwt_token");

    if(storedName) {
		window.location.href = "http://localhost:7341/profile/"; // "http://noob3-4.tghack.no/profile/" + username + '/';
    }
}

function login() {
	var username = document.getElementById("username").value;
	var password = document.getElementById("password").value;

	if (username == "") {
		var error = "Please write at least an username.";
		document.getElementById('error-msg').innerHTML = '<span id="error-msg">' + error + '</span>'
		return;
	}

	var data = JSON.stringify({
		username: username,
		password: password
	});

	const url = base_url + "/login";

	var xhr = new XMLHttpRequest();
	xhr.open("POST", url);
	xhr.setRequestHeader("Content-type", "application/json");
	xhr.onreadystatechange = function () {
		if (xhr.readyState == 4 && xhr.status == 200) {
			var result = JSON.parse(xhr.responseText);
			if (result.flag) {
				window.alert("Congratulations! Here is your flag: " + result.flag);
			} else {
				console.log("storing: ", result.token, result.user)
				window.sessionStorage.setItem("jwt_token", result.token);
				window.sessionStorage.setItem("username", result.user);
				window.location.href = "http://localhost:7341/profile/"; // "http://noob3-4.tghack.no/profile/" + username + '/';
			}
		}
	}
	xhr.send(data);
}
const url = "http://localhost:7321/";

function login() {
	var username = document.getElementById("username").value;
	var password = document.getElementById("password").value;

	window.location.href = url + "home?user=" + username;
}
const base_url = "http://localhost:7312/api";

function login() {
	var username = document.getElementById("username").value;
	var password = document.getElementById("password").value;
	var data = JSON.stringify({
		username: username,
		password: password
	});

	const url = base_url + "/login";

	var xhr = new XMLHttpRequest();
	xhr.open("POST", url);
	xhr.setRequestHeader("Content-type", "application/json");
	xhr.onreadystatechange = function () {
		var result = JSON.parse(xhr.responseText);
		/*if (xhr.readyState == 4 && xhr.status == 200) {
			var result = JSON.parse(xhr.responseText);
			if (result.flag) {
				document.write("Congratulations! Here is your flag: " + result.flag);
				return;
			}
		}*/
		if (xhr.readyState == 4) {
			document.write(result.msg);
		}
	}
	xhr.send(data);
}
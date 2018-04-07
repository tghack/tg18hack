const express = require('express')
const jwt = require('jsonwebtoken')
const path = require('path')
const md5 = require('md5');
const constants = require('./constants').constants;
const jwtSecret = require('./constants').jwtSecret;
const flag = require('./constants').flag;

const app = express();
const apiRoutes = express.Router();
const TOKEN_VALID_MINUTES = 60000;

apiRoutes.get('/health', (req, res) => {
	res.status(200).send("Server working..");
})

apiRoutes.post('/login', (req, res) => {
	const username = req.body.username;

	const exp = Math.floor(Date.now() / 1000) + (TOKEN_VALID_MINUTES * 60);
	const expDate = new Date();
	expDate.setMinutes(expDate.getMinutes() + TOKEN_VALID_MINUTES);
	const payload = {
		"iat": Date.now(),
		"exp": exp, // 30 minutes
		"sub": username,
		"iss": "maritio_o"
	};
	const token = jwt.sign(payload, jwtSecret);

	res.setHeader('X-Access-Token', token);
	res.status(200).send({ user: username, token: token, exp: expDate });
});

apiRoutes.patch('/profile', (req, res) => {
	var token = req.body.token || req.query.token || req.headers['x-access-token'];

	const username = req.body.username;
	const os = req.body.os;
	const text_editor = req.body.text_editor;

	if (text_editor.toLowerCase() == "emacs") {
		res.status(401).send({"error": "Oh noes, Emacs users are unauthorized."})
	}

	if (token) {
		user = {};
		jwt.verify(token, jwtSecret, { ignoreExpiration: false, algorithms: ['HS256'] }, (e, decoded) => {
			if (e) {
				res.status(401).send("Token not verified. Are you trying to falsify a token? :O");
				return;
			} else {
				user = decoded.sub;

				if (user == 'admin') {
					res.status(200).send({ "flag": flag });
					return;
				}
			}
		});
	}

	res.status(200).send({});
});


module.exports = apiRoutes;
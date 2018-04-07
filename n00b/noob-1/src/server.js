const express = require('express')
const jwt = require('jsonwebtoken')
const path = require('path')

const app = express()

app.get('/', (req, res) => res.sendFile(path.join(__dirname+'/index.html')))



app.get('/login/:username', (req, res) => {
	const username = req.params.username;
	const payload = {
		"sub": username,
		"iat": Date.now(),
		"exp": Math.floor(Date.now() / 1000) + (5*24*60*60),
		"iss": 'maritio_o', 
		"aud": 'TG18{any0ne_using_this_token_can_pretend_to_be_y0u_0n_this_website}'
	};
	const token = jwt.sign(payload, 'supersecretsigningsecret');
	res.set('x-access-token', token);
	res.cookie("TG18{r3m3mb3r_k1ds_d0nt_put_valu4bl3_1nf0rm4t10n_1n_th3_he4ders_w1th0ut_pr0tect10n}");
	res.status(200).send("Welcome, " + username + "!");
})

app.get('/secretpage', (req, res) => {
	var responsetext = "<h1>Shhh.. This is a secret page. Here, I keep my deepest secrets...</h1>" +
		"<h2>TG18{pages_that_can_be_bruteforced_are_not_so_secret}<h2>";
	res.status(200).send(responsetext);
})

app.use(express.static('public'));
app.listen(7171, () => console.log('Example app listening on port 7171!'))



const express = require('express')
const path = require('path')
const app = express()

const port = 7321;

app.get('/', (req, res) => res.sendFile(path.join(__dirname+'/index.html')))

app.get('/home/:user?', (req, res) => {
    const user = req.query.user;
    if(user == "admin") {
        res.send("<h1>Welcome, admin!</h1> <h3>Here is your flag: TG18{sometimes, you can find others' pages by changing an ID or some personal value in the URL}</h3>");
    }

    res.send("<h1>Welcome, " + user + "</h1> <h3>Unfortunately, there is nothing for you to see here :(</h3>");
});

app.use(express.static('public'));
app.listen(port, () => console.log('Example app listening on port %s!', port))
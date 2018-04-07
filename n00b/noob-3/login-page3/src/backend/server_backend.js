const express = require('express');
const fs = require('fs');
const api = require('./api');
const cors = require('cors');
const bodyParser = require('body-parser');
const constants = require('./constants').constants;
const flag = require('./constants').flag;

const app = express();

app.use(cors());
app.use(bodyParser.json());
app.use('/api', api);

app.listen(constants.PORT, () => console.log('Example app listening on port %s!', constants.PORT))

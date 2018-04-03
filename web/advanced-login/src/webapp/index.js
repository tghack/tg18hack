const express = require("express");
const app = express();
const path = require("path");
const bodyParser = require('body-parser');

var MongoClient = require("mongodb").MongoClient;
var url = "mongodb://127.0.0.1:27017";

MongoClient.connect(url, function(err, db) {
    if(err){
        console.log(err);
        return;
    }

    db.db("tghack-mongo").dropDatabase(callback=function(err, res){
        var mydb = db.db("tghack-mongo");

        mydb.createCollection("users", function(err, collection) {
            collection.insertOne({user: "admin", pass: "SXqCpCXqgDTIDDooV#pfllBAGfd?pI"}, function(err, res) {
                app.post("/login", function (req, res) {
                    collection.findOne({user: req.body.user, pass: req.body.pass}, function (err, users) {
                        if(err || (users == null)){
                            res.status(403);
                            res.send("ACCESS DENIED");
                        }else{
                            res.send("Flag: TG18{NoSQL_doesn't_mean_not_injectable}");
                        }
                    });
                });
            });
        });
    });
    app.get("/", function (req, res) {
        res.sendFile(path.join(__dirname + "/index.html"));
    });
    app.get("/script.js", function (req, res) {
        res.sendFile(path.join(__dirname + "/script.js"));
    });
    app.get("/style.css", function (req, res) {
        res.sendFile(path.join(__dirname + "/style.css"));
    });
    app.use(bodyParser.json({type: (req) => true}));
    app.listen(1332, "0.0.0.0");
});

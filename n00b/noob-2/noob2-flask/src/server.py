from flask import Flask, request, redirect

app = Flask(__name__, static_url_path="")

@app.route("/robots.txt")
def robots():
    return "User-agent: *\nDisallow: /flag"

@app.route("/flag")
def flag():
    print "flag yo"
    return "TG18{bip_bop_n0_crawl3rs_allow3d}"

@app.route("/style.css")
def style():
    return app.send_static_file("style.css")

@app.route("/")
@app.route("/index.html")
def index():
    return app.send_static_file("./index.html")

@app.route("/pictures/flags/")
def root():
    name = request.args.get("name")
    if not name:
        return 404

    path = "./pictures/flags/" + name
    with open(path, "rb") as f:
        data = f.read()
    return data

if __name__ == "__main__":
    app.run(debug=True)

package main

import (
	"os"
	"os/exec"
)

func main() {
	mongo := exec.Command("/usr/bin/mongod", "--config", "/hack/mongodb/mongodb.conf")
	node := exec.Command("/usr/bin/node", "/hack/webapp/index.js")

	go waitExec(mongo)
	waitExec(node)
}

func waitExec(cmd *exec.Cmd) {
	for {
		cmd.Stdout = os.Stdout
		cmd.Stderr = os.Stderr
		cmd.Run()
	}
}

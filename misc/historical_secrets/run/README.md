## The task
Participants will only need access to `historical_secrets.tgz`.

## Making changes to the task
The source files for this task lives in the standard src folder. Because this
is a git task and our tasks are stored in git, we cannot have a `.git` folder
in a task, and that's why there is a `git` folder without the leading dot.

To update the task, rename the `git` folder to `.git` and do your stuff.
`cd` out of src and `tar -czf run/historical_secrets.tgz run`.
This will overwrite the old archive.

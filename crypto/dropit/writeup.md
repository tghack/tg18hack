# Writeup Drop it

Based on the wording in the task description and the `=` at the end of the string,
we guess that this is base64 encoded.

Decoding it gives:
```bash
> echo -n VEcxOHthbGxfeW91cl9iYXNlX2FyZV9ub3dfZHJvcHBlZH0= | base64 -D
TG18{all_your_base_are_now_dropped}
```

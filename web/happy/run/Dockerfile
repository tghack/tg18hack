FROM ubuntu:16.04

RUN apt update && apt install -y nginx php-fpm && rm /etc/nginx/sites-enabled/default
COPY src /var/www/happy
COPY happy.nginx /etc/nginx/sites-enabled/happy
RUN useradd -m bob
COPY run/my_secret_folder /home/bob/.my_secret_folder
COPY run/bash_history /home/bob/.bash_history

ENTRYPOINT service nginx start && service php7.0-fpm start && /bin/bash

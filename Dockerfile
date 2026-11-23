FROM gcc:latest
WORKDIR /app
COPY . .
RUN g++ -o user_app user.cpp
CMD ["./user_app"]
CREATE TABLE users (
    id serial PRIMARY KEY,
    username character varying(255) NOT NULL,
    email character varying(255) UNIQUE NOT NULL,
    password character varying(255) NOT NULL,
    friend_ids bigint[]
);

CREATE TABLE messages (
    message_id serial PRIMARY KEY,
    sender_id integer NOT NULL,
    receiver_id integer NOT NULL,
    message_text text,
    sent_at timestamp with time zone DEFAULT current_timestamp,
    CONSTRAINT fk_sender FOREIGN KEY (sender_id) REFERENCES users(id),
    CONSTRAINT fk_receiver FOREIGN KEY (receiver_id) REFERENCES users(id)
);

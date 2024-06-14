-- Messenger database schema. Idempotent: safe to run on an existing database.
-- Usage: psql "$DATABASE_URL" -f db/schema.sql

CREATE TABLE IF NOT EXISTS users (
    id            bigserial    PRIMARY KEY,
    username      varchar(20)  NOT NULL CHECK (length(btrim(username)) > 0),
    email         varchar(254) NOT NULL,
    password_hash text         NOT NULL,
    created_at    timestamptz  NOT NULL DEFAULT now()
);
-- Emails are compared case-insensitively.
CREATE UNIQUE INDEX IF NOT EXISTS users_email_lower_key ON users (lower(email));

-- Login sessions. Only the SHA-256 of the token is stored, so a leaked table does not give access.
CREATE TABLE IF NOT EXISTS sessions (
    token_hash char(64)    PRIMARY KEY,
    user_id    bigint      NOT NULL REFERENCES users (id) ON DELETE CASCADE,
    created_at timestamptz NOT NULL DEFAULT now(),
    expires_at timestamptz NOT NULL
);
CREATE INDEX IF NOT EXISTS sessions_user_id_idx ON sessions (user_id);

-- Friendship is mutual: adding a friend inserts both (a, b) and (b, a).
CREATE TABLE IF NOT EXISTS friendships (
    user_id    bigint      NOT NULL REFERENCES users (id) ON DELETE CASCADE,
    friend_id  bigint      NOT NULL REFERENCES users (id) ON DELETE CASCADE,
    created_at timestamptz NOT NULL DEFAULT now(),
    PRIMARY KEY (user_id, friend_id),
    CHECK (user_id <> friend_id)
);

CREATE TABLE IF NOT EXISTS messages (
    id          bigserial   PRIMARY KEY,
    sender_id   bigint      NOT NULL REFERENCES users (id) ON DELETE CASCADE,
    receiver_id bigint      NOT NULL REFERENCES users (id) ON DELETE CASCADE,
    body        text        NOT NULL CHECK (length(body) BETWEEN 1 AND 4000),
    sent_at     timestamptz NOT NULL DEFAULT now()
);
-- History of one conversation, newest first, regardless of who sent what.
CREATE INDEX IF NOT EXISTS messages_conversation_idx
    ON messages (LEAST(sender_id, receiver_id), GREATEST(sender_id, receiver_id), id DESC);

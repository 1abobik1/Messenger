import {useEffect, useState} from 'react';
import {WS_URL} from '../api';

const MAX_RECONNECT_DELAY_MS = 10000;

// Keeps a WebSocket to /ws open while the user is signed in and reconnects with exponential backoff.
// Returns the current socket (null while connecting).
export default function useChatSocket(token) {
  const [socket, setSocket] = useState(null);

  useEffect(() => {
    if (!token) {
      return undefined;
    }
    let ws = null;
    let attempt = 0;
    let reconnectTimer = null;
    let stopped = false;

    const connect = () => {
      ws = new WebSocket(`${WS_URL}?token=${encodeURIComponent(token)}`);
      ws.onopen = () => {
        attempt = 0;
        setSocket(ws);
      };
      ws.onclose = () => {
        setSocket(null);
        if (!stopped) {
          const delay = Math.min(1000 * 2 ** attempt, MAX_RECONNECT_DELAY_MS);
          attempt += 1;
          reconnectTimer = setTimeout(connect, delay);
        }
      };
    };
    connect();

    return () => {
      stopped = true;
      clearTimeout(reconnectTimer);
      ws?.close();
    };
  }, [token]);

  return socket;
}

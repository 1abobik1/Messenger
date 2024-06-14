import React, {createContext, useCallback, useMemo, useState} from 'react';
import {apiFetch} from '../api';

export const AuthContext = createContext(null);

const SESSION_KEY = 'session';

function loadSession() {
  try {
    const session = JSON.parse(localStorage.getItem(SESSION_KEY));
    return session?.token && session?.user ? session : null;
  } catch {
    return null;
  }
}

export const AuthProvider = ({children}) => {
  // {token, user: {id, username, email}} as returned by /api/auth/login and /api/auth/signup
  const [session, setSession] = useState(loadSession);

  const signin = useCallback((newSession, cb) => {
    localStorage.setItem(SESSION_KEY, JSON.stringify(newSession));
    setSession(newSession);
    cb?.();
  }, []);

  const signout = useCallback((cb) => {
    localStorage.removeItem(SESSION_KEY);
    setSession(null);
    cb?.();
  }, []);

  // apiFetch with the current token; an expired session sends the user back to the login page.
  const request = useCallback(
    async (path, options = {}) => {
      try {
        return await apiFetch(path, {...options, token: session?.token});
      } catch (error) {
        if (error.status === 401) {
          signout();
        }
        throw error;
      }
    },
    [session, signout],
  );

  const value = useMemo(
    () => ({
      user: session?.user ?? null,
      token: session?.token ?? null,
      signin,
      signout,
      request,
    }),
    [session, signin, signout, request],
  );

  return <AuthContext.Provider value={value}>{children}</AuthContext.Provider>;
};

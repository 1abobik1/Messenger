// Base URL of the C++ server; set REACT_APP_API_URL at build time to point elsewhere.
export const API_URL = (process.env.REACT_APP_API_URL || 'http://localhost:9000').replace(/\/$/, '');
export const WS_URL = `${API_URL.replace(/^http/, 'ws')}/ws`;

export class ApiError extends Error {
  constructor(status, message) {
    super(message);
    this.status = status;
  }
}

// Calls the REST API. Sends `body` as JSON and the session token as a Bearer header.
// Throws ApiError with the server's error message for non-2xx responses.
export async function apiFetch(path, {method = 'GET', body, token} = {}) {
  const headers = {};
  if (body !== undefined) {
    headers['Content-Type'] = 'application/json';
  }
  if (token) {
    headers.Authorization = `Bearer ${token}`;
  }

  let response;
  try {
    response = await fetch(API_URL + path, {
      method,
      headers,
      body: body === undefined ? undefined : JSON.stringify(body),
    });
  } catch {
    throw new ApiError(0, 'The server is not available');
  }

  const data = response.status === 204 ? null : await response.json().catch(() => null);
  if (!response.ok) {
    throw new ApiError(response.status, data?.error || `Request failed (${response.status})`);
  }
  return data;
}

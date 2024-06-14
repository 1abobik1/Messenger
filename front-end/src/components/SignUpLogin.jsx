import React, {useState} from 'react';
import '../css/sign.css';
import {Navigate, useNavigate} from "react-router-dom";
import useAuth from "../auth/useAuth";
import {apiFetch} from "../api";

function isValidEmail(email) {
  const emailPattern = /^[^\s@]+@[^\s@]+\.[^\s@]+$/;
  return emailPattern.test(email.trim());
}

function isValidPassword(password) {
  return password.length >= 10;
}

function isValidNickname(nickname) {
  return (nickname.length <= 20) && (nickname.length > 0) && !/^\d+$/.test(nickname);
}

// Returns the first problem with the sign-up form, or '' if it looks valid (the server checks again).
function signUpError(userName, email, password) {
  if (!isValidNickname(userName.trim())) {
    return 'Name: 1-20 characters, not only digits';
  }
  if (!isValidEmail(email)) {
    return 'Invalid email';
  }
  if (!isValidPassword(password)) {
    return 'Password must be at least 10 characters';
  }
  return '';
}

const SignUpLogin = () => {
  const [userName, setUserName] = useState('');
  const [email, setEmail] = useState('');
  const [password, setPassword] = useState('');
  const [signUpMessage, setSignUpMessage] = useState('');
  const [emailLogin, setEmailLogin] = useState('');
  const [passwordLogin, setPasswordLogin] = useState('');
  const [loginMessage, setLoginMessage] = useState('');
  const [busy, setBusy] = useState(false);
  const navigate = useNavigate();
  const {user, signin} = useAuth();

  if (user) {
    return <Navigate to='/client' replace={true}/>;
  }

  const authenticate = (path, body, setMessage) => {
    setBusy(true);
    setMessage('');
    apiFetch(path, {method: 'POST', body})
      .then(session => signin(session, () => navigate('/client', {replace: true})))
      .catch(error => {
        setMessage(error.message);
        setBusy(false);
      });
  };

  const handleSignUp = () => {
    const problem = signUpError(userName, email, password);
    if (problem) {
      setSignUpMessage(problem);
      return;
    }
    // The password is sent as is and hashed with bcrypt on the server.
    authenticate('/api/auth/signup', {username: userName.trim(), email, password}, setSignUpMessage);
  };

  const handleLogin = () => {
    if (!isValidEmail(emailLogin) || passwordLogin.length === 0) {
      setLoginMessage('Enter your email and password');
      return;
    }
    authenticate('/api/auth/login', {email: emailLogin, password: passwordLogin}, setLoginMessage);
  };

  return (
    <div className="body">
      <div className="main">
        <input type="checkbox" id="chk" aria-hidden="true"/>

        <div className="signup" id="signup_id">
          <form id="signup_form">
            <label htmlFor="chk" aria-hidden="true">Sign up</label>
            <input type="text" name="user_name_sign" placeholder="User name" required={true}
                   value={userName}
                   minLength={1}
                   onChange={(e) => setUserName(e.target.value)}/>
            <input type="email" name="email_sign" placeholder="Email" required={true}
                   value={email}
                   onChange={(e) => setEmail(e.target.value)}/>
            <input type="password" name="pswd_sign" placeholder="Password" required={true}
                   minLength={10}
                   value={password}
                   onChange={(e) => setPassword(e.target.value)}/>
            {signUpMessage && <p className="form-error">{signUpMessage}</p>}
            <button type="button" id="signup_button" onClick={handleSignUp} disabled={busy}
                    className='hover:bg-indigo-900'>Sign up</button>
          </form>
        </div>

        <div className="login" id="login_id">
          <form id="login_form">
            <label htmlFor="chk" aria-hidden="true">Login</label>
            <input type="email" name="email_log" placeholder="Email" required={true}
                   value={emailLogin}
                   onChange={(e) => setEmailLogin(e.target.value)}/>
            <input type="password" name="pswd_log" placeholder="Password" required={true}
                   value={passwordLogin}
                   onChange={(e) => setPasswordLogin(e.target.value)}/>
            {loginMessage && <p className="form-error">{loginMessage}</p>}
            <button type="button" id="login_button" onClick={handleLogin} disabled={busy}
                    className='hover:bg-indigo-900'>Login</button>
          </form>
        </div>
      </div>
    </div>
  );
};

export default SignUpLogin;

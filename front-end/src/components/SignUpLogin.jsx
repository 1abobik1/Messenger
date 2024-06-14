import React, {useState} from 'react';
import '../css/sign.css';
import {useNavigate} from "react-router-dom";
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

const SignUpLogin = () => {
  const [userName, setUserName] = useState('');
  const [email, setEmail] = useState('');
  const [password, setPassword] = useState('');
  const navigate = useNavigate();
  const {signin} = useAuth();

  const handleSignUp = () => {
    if (isValidNickname(userName) && isValidEmail(email) && isValidPassword(password)) {
      // The password is sent as is and hashed with bcrypt on the server.
      apiFetch('/api/auth/signup', {method: 'POST', body: {username: userName, email, password}})
        .then(session => signin(session, () => navigate('/client', {replace: true})))
        .catch(error => alert(error.message));
    } else {
      if (!isValidNickname(userName)) {
        alert("your name should be short and contain more than just numbers");
      }
      if (!isValidEmail(email)) {
        alert("Invalid email");
      }
      if (!isValidPassword(password)) {
        alert("Your password must be at least 10 characters");
      }
    }
  };

  const [emailLogin, setEmailLogin] = useState('');
  const [passwordLogin, setPasswordLogin] = useState('');

  const handleLogin = () => {
    if (isValidEmail(emailLogin) && passwordLogin.length > 0) {
      apiFetch('/api/auth/login', {method: 'POST', body: {email: emailLogin, password: passwordLogin}})
        .then(session => signin(session, () => navigate('/client', {replace: true})))
        .catch(error => alert(error.status === 401 ? "Wrong email or password" : error.message));
    } else {
      alert("Wrong email or password");
    }
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
            <button type="button" id="signup_button" onClick={handleSignUp} className='hover:bg-indigo-900'>Sign up</button>
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
            <button type="button" id="login_button" onClick={handleLogin} className='hover:bg-indigo-900'>Login</button>
          </form>
        </div>
      </div>
    </div>
  );
};

export default SignUpLogin;

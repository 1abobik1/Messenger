import React from 'react';
import '../css/sign.css'

const SignUpLogin = () => {
  return (
    <div className="body">
      <div className="main">
        <input type="checkbox" id="chk" aria-hidden="true"/>

        <div className="signup" id="signup_id">
          <form id="signup_form">
            <label htmlFor="chk" aria-hidden="true">Sign up</label>
            <input type="text" name="user_name_sign" placeholder="User name" required=""/>
            <input type="email" name="email_sign" placeholder="Email" required=""/>
            <input type="password" name="pswd_sign" placeholder="Password" required=""/>
            <button type="button" id="signup_button">Sign up</button>
          </form>
        </div>

        <div className="login" id="login_id">
          <form id="login_form">
            <label htmlFor="chk" aria-hidden="true">Login</label>
            <input type="email" name="email_log" placeholder="Email" required=""/>
            <input type="password" name="pswd_log" placeholder="Password" required=""/>
            <button type="button" id="login_button">Login</button>
          </form>
        </div>
      </div>
    </div>


  );
};

export default SignUpLogin;

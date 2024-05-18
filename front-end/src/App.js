import './App.css';
import Client from "./components/Client";
import SignUpLogin from "./components/SignUpLogin";
import {Route, Routes} from "react-router-dom";
import RequireAuth from "./auth/RequireAuth";
import {AuthProvider} from "./auth/AuthProvider";
import NotFound from "./components/NotFound";
import ClientId from "./components/ClientId";
import URLGuard from "./components/URLGuard";


function App() {
  return (
    <div className="App">
      <AuthProvider>
        <Routes>
          <Route path="/client" element={<RequireAuth><URLGuard><Client/></URLGuard></RequireAuth>}>
            <Route path=":id" element={<RequireAuth><URLGuard><ClientId/></URLGuard></RequireAuth>}/>
          </Route>
          <Route path="/" element={<SignUpLogin/>}/>
          <Route path="*" element={<NotFound/>}/>
        </Routes>
      </AuthProvider>
    </div>
  );
}

export default App;

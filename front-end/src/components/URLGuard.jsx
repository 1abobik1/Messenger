import {useEffect} from 'react';
import {useNavigate} from 'react-router-dom';

export const allowedRoutes = new Set(['/client', '/']);

const URLGuard = ({children}) => {
  const navigate = useNavigate();

  useEffect(() => {


    if (!allowedRoutes.has(window.location.pathname)) {
      navigate('/client');
    }
  }, [navigate]);

  return children;
};

export default URLGuard;


% Kalman filter implementation
function [x, p] = Kalman_example(t, h, a, s, e)
% Inputs
% t = time array (in s)
% h = position array (in m)
% a = acceleration array (in m/s^2)
% s = starting index for the arrays
% e = ending index for the arrays

% These are the model parameters and don't need to be changed
C = [1 0];
x0 = [0; 0];
p0 = [10; 0 ; 0; 1]; % first number error in position squared ?
R = 2; % error in position squared ?
Q = 10; % error in acceleration squared ?

% Things to save
x = NaN(2, length(s:e));
p = NaN(4,length(s:e));
x(:, 1) = x0;
p(:, 1) = p0;

% Do the Calculation
for ii = s:(e-1)
    % Getting some values
    dt = t(ii + 1) - t(ii);
    A = [1 dt; 0 1];
    B = [0.5*dt^2; dt];
    P = [p(1, ii-s+1), p(2, ii-s+1); p(3, ii-s+1), p(4, ii-s+1)];
    
    % Prediction step
    x_pred = A * x(:, ii-s+1) + B * a(ii + 1);
    p_pred = A * P * A' + Q;
    
    % Update step
    K = (p_pred * C') / (C * p_pred * C' + R);
    x(:, ii+1-s+1) = x_pred + K * (h(ii + 1) - C*x_pred);
    p_new = (eye(2) - K * C) * p_pred;
    p(:, ii+1-s+1) = [p_new(1, 1); p_new(1, 2); p_new(2, 1); p_new(2, 2)];
    
end

end
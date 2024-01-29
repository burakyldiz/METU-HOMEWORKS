N= 1509;
lambda_bulk = 50;
lambda_container = 40;
lambda_oil = 25;

totalWeight = zeros(N,1);

for k=1:N
    U = rand; i=0;
    F = exp(-lambda_bulk);
    while (U>=F)
        i=i+1;
        F = F + exp(-lambda_bulk)*lambda_bulk^i/gamma(i+1);
    end
    number_bulk = i;
    lambda_weight_bulk = 0.1;
    alpha_bulk = 60;
    weight_bulk = sum(-1/lambda_weight_bulk * log(rand(alpha_bulk,number_bulk)));
    total_weight_bulk = sum(weight_bulk);
    
    U = rand; i=0;
    F = exp(-lambda_container);
    while (U>=F)
        i=i+1;
        F = F + exp(-lambda_container)*lambda_container^i/gamma(i+1);
    end
    number_container = i;
    lambda_weight_container = 0.05;
    alpha_container = 100;
    weight_container = sum(-1/lambda_weight_container * log(rand(alpha_container,number_container)));
    total_weight_container = sum(weight_container);
    
    U = rand; i=0;
    F = exp(-lambda_oil);
    while (U>=F)
        i=i+1;
        F = F + exp(-lambda_oil)*lambda_oil^i/gamma(i+1);
    end
    number_oil = i;
    lambda_weight_oil = 0.02;
    alpha_oil = 120;
    weight_oil = sum(-1/lambda_weight_oil * log(rand(alpha_oil,number_oil)));
    total_weight_oil = sum(weight_oil);
    
    
    totalWeight(k) = total_weight_bulk + total_weight_container + total_weight_oil;
    
end

p_est = mean(totalWeight>300000);
expectedWeight = mean(totalWeight);
stdWeight = std(totalWeight);
fprintf('Estimated probability = %f\n',p_est);
fprintf('Expected weight = %f\n',expectedWeight);
fprintf('Standard deviation = %f\n',stdWeight);

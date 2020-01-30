Bernoulli Distribution
==========================

Demonstration of the Bernoulli distribution. 

## Show Distribution

Displays theoretical Bernoulli distribution, given specified parameter values.

### Parameter
- p: Probability of "succes"

### Display

- Explanatory text: Displays explanatory text
- Parameters, support, and moments: Displays the definition of parameters, the support of the random variable, and the moments of the theoretical distribution
- Probability mass function: Displays the probability mass plot
- Cumulative distribution function: Displays the cumulative distribution plot


## Generate and Display Data
- Variable name: Specify the name of the variable. Once filled, creates an empty column in the current data set.
- Number of samples: Specify the number of samples.
- Draw samples: Sample from the theoretical distribution.

- Get variable from data set: Select a variable to display and fit with the theoretical distribution.

### Statistics
- Descriptives: Displays a descriptive table of the selected variable.
- First ... moments: Displays a table with the raw and central sample moments of the selected variable. Defaults to first 2 moments.

### Plots
- Bar plot: Display a bar plot of the selected variable.
- Empirical cumulative distribution: Displays an empirical cumulative distribution plot of the selected variable.

## Estimate Parameters
- Maximum likelihood: Fit the theoretical distribution to the selected variable by maximizing the likelihood function. The parameter values specified in Show distribution section serve as starting points for the procedure.

### Output
- Estimates: Displays a table with the parameter estimates. Changing parametrization changes which parameters are displayed.
	- Std. error: Displays the standard error of the parameter estimates
	- Confidence interval: Displays the confidence interval of the parameters at the specified confidence level.


## Assess Fit

### Plots
- Histogram vs. theoretical pdf: Displays a histogram of the selected variable overlayed with the probability density function of the fitted distribution
- Empirical vs. theoretical cdf: Displays an empirical cumulative distribution plot overlayed with the cumulative distribution function of the fitted distribution
- Q-Q plot: Displays the quantile-quantile plot. The *x*-axis shows the theoretical quantiles of the data points under the fitted distribution, the *y*-axis shows the empirical quantiles of the selected variable.
- P-P plot: Displays the probability-probability plot. The *x*-axis shows the theoretical value of the cumulative density function of the data points under the fitted distribution, the *y*-axis shows the empirical percentiles of the selected variable.

### Statistics
- Chi-square: Displays the chi-square goodness of fit test
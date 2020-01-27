Inverse Gamma Distribution
==========================

Demonstration of the Inverse Gamma distribution. 

## Show Distribution

Displays theoretical Inverse Gamma distribution, given specified parameter values.

### Parameters

- k, &theta;: The Inverse Gamma distribution is specified using the shape and scale parameters 
- &alpha;, &beta;: The Inverse Gamma distribution is specified using the shape and rate parameters
- k, &mu;: The Inverse Gamma distribution is specified using the shape and mean parameters

### Display

- Explanatory text: Displays explanatory text
- Parameters, support, and moments: Displays the definition of parameters, the support of the random variable, and the moments of the theoretical distribution
- Probability density function: Displays the probability density plot
- Cumulative distribution function: Displays the cumulative distribution plot
- Quantile function: Displays the quantile plot

### Options

- Range of x from ... to ... : Defines the limits of the x-axis of the probability density plot and cumulative distribution plot, and the limits of the y-axis of the quantile plot.
- Highlight density: Highlights the probability density on the probability density plot and cumulative distribution plot at specified values of x
- Highlight probability: Highlights the probability in between the specified values of x in the density plot (area under the curve), and highlights the cumulative probability at the specified values in the cumulative distribution plot
- Interval: Select the bounds of the interval to display: Density is highlighted at the lower and upper bounds, the probability is displayed for the specified interval.

## Generate and Display Data
- Variable name: Specify the name of the variable. Once filled, creates an empty column in the current data set.
- Number of samples: Specify the number of samples.
- Draw samples: Sample from the theoretical distribution.

- Get variable from data set: Select a variable to display and fit with the theoretical distribution.

### Statistics
- Descriptives: Displays a descriptive table of the selected variable.
- First ... moments: Displays a table with the raw and central sample moments of the selected variable. Defaults to first 2 moments.

### Plots
- Histogram with ... bins: Display a histogram of the selected variable with the number of specified bins.
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
- Kolmogorov-Smirnov: Displays the Kolmogorov-Smirnov test
- Cramer von Misses: Displays the Cramer von Misses test
- Anderson-Darling: Displays the Anderson-Darling test
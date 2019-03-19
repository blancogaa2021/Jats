How to Write R Analyses for JASP
================================

R code for JASP should follow JASP's [R style guide](https://github.com/jasp-stats/jasp-desktop/blob/development/Docs/development/r-style-guide.md). 

This document will guide you through the process of writing an R analysis for JASP. Two things should be noted before we get started. First, this guide assumes that you have knowledge about basic R concepts such as functions. Second, writing an R analysis is necessary but **not** sufficient to create a new module for JASP. For this goal, other files (such as [the QML file for the interface](jasp-qml-guide.md)) need to be created.

Every JASP R analysis will consist of several types of functions:
1. a single main analysis function that organizes the analysis and its output,
2. one or multiple create functions that create output elements (such as tables and plots), and
3. one or multiple fill up functions that compute results and fill the output elements.

In the remainder of this document, you will learn how to write these types of functions. Explanations will be illustrated using excerpts from a few JASP analyses, mainly the relatively simple Binomial Test. If you're writing a simple analysis it may suffice to work through steps 1 to 6. During these steps you will learn how to write a straightforward analysis that contains a table and a plot. In the two sections at the end of this guide we'll delve into (1) how multiple related tables or plots may be grouped together and (2) how we write analyses whose tables and plots revolve around a single computed model (as opposed to separately calculating results for each table and/or plot).

Table of Contents:
- [Step 1 - Creating the Main Analysis Function](#step-1---creating-the-main-analysis-function)
  * [Step 1.1 - The Arguments](#step-11---the-arguments)
- [Step 2 - Setting a Title](#step-2---setting-a-title)
- [Step 3 - Checking if Results can be Computed](#step-3---checking-if-results-can-be-computed)
- [Step 4 - Reading the Dataset](#step-4---reading-the-dataset)
- [Step 5 - Checking for Errors](#step-5---checking-for-errors)
- [Step 6 - Creating Output Tables and Plots](#step-6---creating-output-tables-and-plots)
  * [Step 6.1 - Tables](#step-61---tables)
    + [Step 6.1.1 - Creating a JASP Table](#step-611---creating-a-jasp-table)
    + [Step 6.1.2 - Dependencies](#step-612---dependencies)
    + [Step 6.1.3 - Citations](#step-613---citations)
    + [Step 6.1.4 - Column Specification](#step-614---column-specification)
    + [Step 6.1.5 - Expected Number of Rows](#step-615---expected-number-of-rows)
    + [Step 6.1.6 - Adding Footnotes](#step-616---adding-footnotes)
    + [Step 6.1.7 - Adding the Table to the Output](#step-617---adding-the-table-to-the-output)
    + [Step 6.1.8 - Filling the table](#step-618---filling-the-table)
    + [Step 6.1.9 - Reporting Errors](#step-619---reporting-errors)
    + [Step 6.1.8 - Multiple Tables](#step-618---multiple-tables)
  * [Step 6.2 - Plots](#step-62---plots)
    + [Step 6.2.1 - Creating a JASP Plot](#step-621---creating-a-jasp-plot)
    + [Step 6.2.2 - Dependencies](#step-622---dependencies)
    + [Step 6.2.3 - Citations](#step-623---citations)
    + [Step 6.2.4 - Adding Footnotes](#step-624---adding-footnotes)
    + [Step 6.2.5 - Adding the Plot to the Output](#step-625---adding-the-plot-to-the-output)
    + [Step 6.2.6 - Filling the Plot](#step-626---filling-the-plot)
    + [Step 6.2.7 - Reporting Errors](#step-627---reporting-errors)
    + [Step 6.2.8 - Multiple Plots](#step-628---multiple-plots)
- [ADDENDUM I - Grouping Multiple Output Elements Together](#addendum-i---grouping-multiple-output-elements-together)
  * [Creating a JASP Container](#creating-a-jasp-container)
  * [Using a Container](#using-a-container)
  * [Dependencies](#dependencies)
    + [Value Remains in Option](#value-remains-in-option)
  * [Propagating Errors Through a Container](#propagating-errors-through-a-container)
- [ADDENDUM II - Reusing Results for Multiple Output Elements](#addendum-ii---reusing-results-for-multiple-output-elements)
  * [Creating a JASP State](#creating-a-jasp-state)
  * [Dependencies](#dependencies-1)
  * [Computing Results](#computing-results)
  * [Storing Results in the State](#storing-results-in-the-state)
  * [Retrieving the State Object](#retrieving-the-state-object)

## Step 1 - Creating the Main Analysis Function
Each analysis in JASP needs a main analysis function. This function will provide an overview of all output elements and steps that are needed to conduct the full analysis. The name of your analysis must match the (case sensitive) name you specified in your description.json file under `"function":`.

<details>
	<summary>Code</summary>
	
  ```r
  BinomialTest <- function() {
  ```
  
</details>

### Step 1.1 - The Arguments
The main analysis function has the following arguments:
- `jaspResults`: the object that will contain all results from this analysis and connects it to the output
- `dataset`: an empty placeholder we will start using in the future when analyses can be run interactively from R through the JASP R package
- `options`: a named list of interface options selected by the user; each name matches the name of a QML component

<details>
	<summary>Code</summary>
	
  ```r
  BinomialTest <- function(jaspResults, dataset, options) {
  ```
  
</details>

## Step 2 - Setting a Title
Each analysis needs a title. This title will be shown at the top of the output.

<details>
  <summary>Code</summary>

  ```r
  BinomialTest <- function(jaspResults, dataset, options) {
  
    jaspResults$title <- "Binomial Test"
  ```
  
</details>

## Step 3 - Checking if Results can be Computed
Analyses usually require a certain minimum input before it can run, for example, an analysis might need at least a dependent variable and an independent variable. If this minimum input is not given, we should still show tables but filled with dots instead of actual results; plots should also be displayed but with empty axes (fortunately, this is quite easy as we'll show later). Consequently, it makes sense to determine early in the analysis whether we are ready to compute the results. In the Binomial Test, at least one variable is needed in order to compute results:

<details>
  <summary>Code</summary>

  ```r
  BinomialTest <- function(jaspResults, dataset, options) {

    jaspResults$title <- "Binomial Test"
    
    ready <- (length(options$variables) > 0)
   ```

</details>

## Step 4 - Reading the Dataset
If your R function is called from JASP the dataset will not be provided directly to the analysis. Instead, you must ask for the dataset. This allows you to specify how variables must be read (e.g., as factor) and how missing values should be treated -- they can be ignored or excluded listwise. To read the dataset, call `.readDataSetToEnd()` with the following optional arguments
- `columns`: columns that must be read without specific coercing to a datatype
- `columns.as.numeric`: columns that must be read as numeric
- `columns.as.ordinal`: columns that must be read as ordinal
- `columns.as.factor`:  columns that must be read as factor
- `all.columns`: boolean specifying if the entire dataset should be read, as opposed to specific columns
- `exclude.na.listwise`: columns where missing values should be deleted listwise

We do have to account for the fact that we do not need data if we're not ready to compute anything (as we verified in [Step 3 - Checking if Results can be Computed](#step-3---checking-if-results-can-be-computed)):

<p><details>
	<summary>Code</summary>
	
  ```r
  BinomialTest <- function(jaspResults, dataset, options) {

    jaspResults$title <- "Binomial Test"
    
    ready <- (length(options$variables) > 0)
    
    if (ready)
      dataset <- .binomReadData(dataset, options)
  ```
   
</details></p>

Where `.binomReadData()` looks like

<p><details>
	<summary>Code</summary>

  ```r
  .binomReadData <- function(dataset, options) {
    if (!is.null(dataset))
      return(dataset)
    else
      return(.readDataSetToEnd(columns.as.factor = options$variables))
  }
  ```

</details></p>

Take note that the column titles in the data.frame returned by `.readDataSetToEnd()` will look a bit jumbled, e.g., a column titled `variable1` would look like `XdmFyaWFibGUx`. This is due to the base64 encoding we perform on the column titles (to avoid crashes due to incompatible names). The problem is that the values in `options$variables` are NOT base64 encoded and therefore do not match the column names in the dataset. Obviously this will present difficulties if we try to subset data later during the computation phase. The way we solve this is by using `.v()` to base64 encode options or `.unv()` to decode column names. To exemplify this, the following would return `TRUE`:

- `"variable1" == .unv("XdmFyaWFibGUx")` 
- `.v("variable1") == "XdmFyaWFibGUx"`

Hence, whenever you wish to match an option to a data.frame column you must encode or decode one of the two. In addition to encoding strings, we also provide `.vf()` and `.unvf()` to base64 encode/decode formulas.

## Step 5 - Checking for Errors
If we have the minimum input our analysis requires, it is important to check for errors that will prevent the results from being computed (e.g., a dependent variable that has no variance). The error checks that should be conducted depend on the analysis. For the Binomial Test, we need to make sure that there is a least one factor level for each variable and that we have at least one observation for each level of the variables. Most common error checks are implemented in the convenience function `.hasErrors()`:

<p><details>
	<summary>Code</summary>
  
  ```r
  BinomialTest <- function(jaspResults, dataset, options) {

    jaspResults$title <- "Binomial Test"
    
    ready <- (length(options$variables) > 0)
    
    if (ready) {
      dataset <- .binomReadData(dataset, options)
      
      .binomCheckErrors(dataset, options)
    }
  ```
  
</details></p>

Where `.binomCheckErrors()` looks like:

<p><details>
	<summary>Code</summary>
	
  ```r
  .binomCheckErrors <- function(dataset, options) {
    # Error Check 1: Number of levels of the variables
    .hasErrors(dataset = dataset, type = "factorLevels",
               factorLevels.target  = options$variables, factorLevels.amount  = "< 1",
               exitAnalysisIfErrors = TRUE)

    # Error check 2: 0 observations for a level of a variable
    for (variable in options$variables) {

      column <- dataset[[.v(variable)]]
      data   <- column[!is.na(column)]
      levels <- levels(data)
  
      for (level in levels) {
        .hasErrors(dataset = data[data == level], type = "observations",
        observations.amount  = "< 1", exitAnalysisIfErrors = TRUE)
      }
      
    }
    
  }
  ```
  
</details></p>

Other common error checks include checking for weird data (too few observations, infinite values, variance is zero), and for a non-positive definite covariance matrix. The argument `exitAnalysisIfErrors` can be used to ensure that if any errors are encountered, JASP will stop the analysis from executing further.

<details>
	<summary>Code</summary>
  
  ```r
    # Error check: Weird data for dependent variable in each level of the grouping variable
    .hasErrors(dataset, type = c('observations', 'variance', 'infinity'),
              all.target = options$variables, all.grouping = options$groupingVariable,
              observations.amount = c('< 3'), exitAnalysisIfErrors = TRUE)
  ```
</details>

<details>
	<summary>Code</summary>
  
  ```r
  # Error check: Check for non-positive definite variance-covariance matrix
  covnwt <- stats::cov
  .hasErrors(dataset, type = 'varCovData',
             varCovData.target = c(options$dependent.variable, options$main.effects.numeric),
             varCovData.corFun = covnwt, exitAnalysisIfErrors = TRUE)
  ```

</details>

## Step 6 - Creating Output Tables and Plots
It is now time to think about our output. What tables and plots do we want to display? In most analyses you will have one main output table that is always shown and then a number of tables and plots that are optional. As a table is almost always shown we will first start explaining how to create it. 

### Step 6.1 - Tables
At this point we start using `jaspResults` which was passed into our function at the start of the analysis. `jaspResults` is used to store our results and helps us figure out if we can re-use any of our tables and plots between calls to our analysis function. Whereas this may sound complex, there is an easy way to check for it: We can just check whether the table we want to make is defined (i.e. not `NULL`) in `jaspResults`. If it is not defined (i.e. `NULL`), the table needs to be created:

<p><details>
	<summary>Code</summary>
	
  ```r
  BinomialTest <- function(jaspResults, dataset, options) {

    jaspResults$title <- "Binomial Test"
    
    ready <- (length(options$variables) > 0)
    
    if (ready) {
	    dataset <- .binomReadData(dataset, options)
      
	    .binomCheckErrors(dataset, options, ready)
    }
    
    if (is.null(jaspResults[["binomialTable"]])
      .binomTableMain(jaspResults, dataset, options, ready)
  ```

</details></p>

Don't worry about the name `"binomialTable"` for now, we'll show where this comes from in [Step 6.1.7 - Adding the Table to the Output](#step-617---adding-the-table-to-the-output).

#### Step 6.1.1 - Creating a JASP Table
Unfortunately, we cannot just create a data.frame and call it a day. There is some markup you'll have to add first; the markup will describes properties of the table so JASP knows how to display it correctly. Let's start by creating a JASP table object and giving it a title that will be displayed in the output:

<details>
	<summary>Code</summary>
  
  ```r
  .binomTableMain(jaspResults, dataset, options, ready) {
    binomialTable <- createJaspTable(title = "Binomial Test")
  ```
  
</details>

#### Step 6.1.2 - Dependencies
Now we need to specify the analysis options on which the table depends. If the value of any of these options changes, the table will be removed from the output and needs to be newly computed. However, if none of these options changes the next time the analysis is called, the table can be reused:

<details>
	<summary>Code</summary>
  
  ```r
  .binomTableMain(jaspResults, dataset, options, ready) {
    binomialTable <- createJaspTable(title = "Binomial Test")
    
    binomialTable$dependOnOptions(c("variables", "testValue", "hypothesis", "confidenceInterval",
                                    "confidenceIntervalInterval", "VovkSellkeMPR"))
  ```
  
</details>

#### Step 6.1.3 - Citations
Many analyses in JASP are based on the work of others and it is important we give them credit. To add citations to a JASP table you can use `$addCitation()`; if no citation is given it defaults to the JASP Team.

<details>
	<summary>Code</summary>
	
  ```r
  .binomTableMain(jaspResults, dataset, options, ready) {
    binomialTable <- createJaspTable(title = "Binomial Test")
    
    binomialTable$dependOnOptions(c("variables", "testValue", "hypothesis", "confidenceInterval",
                                    "confidenceIntervalInterval", "VovkSellkeMPR"))
				    
    binomialTable$addCitation("JASP Team (2018). JASP (Version 0.9.2) [Computer software].")
  ```

</details>

#### Step 6.1.4 - Column Specification
We'll also have to specify what columns our table will have. Some columns are always part of the table. Others depend on whether the user selected the corresponding option in the interface or not. Each column requires a description of what it is and how it should be displayed. The descriptions may contain any of the following (\* denotes required fields):
  - `name`\*: the column identifier
  - `title`\*: displayed at the top of the column; if not specified (set to `""`) the `name` argument is used
  - `type`\*: one of `string`, `number` or `integer`
  - `format`: format specifiers for `type` is `number` (multiple can be specified, separated with semicolons)
      - `dp:X` - format to X decimal places
      - `sf:X` - format to X significant figures
      - `p:X`  - if the value is less than X, substitute `p < X` in it's place (`p:.001` is common)
      - `pc`   - format the number as a percentage (multiply it by 100, and add a % sign) (does not work in conjunction with sf)
  - `combine`: boolean specifying if cells in the column should be merged if they contain the same value
  - `overtitle`: adds a title which is positioned above all columns that specify the same overtitle (often used for confidence intervals, where the lower and upper bound are in separate columns, but the added overtitle groups them together)

<p><details>
	<summary>Code</summary>
  
  ```r
  .binomTableMain(jaspResults, dataset, options, ready) {
    binomialTable <- createJaspTable(title = "Binomial Test")

    binomialTable$dependOnOptions(c("variables", "testValue", "hypothesis", "confidenceInterval",
                                    "confidenceIntervalInterval", "VovkSellkeMPR"))
    
    binomialTable$addCitation("JASP Team (2018). JASP (Version 0.9.2) [Computer software].")
    
    binomialTable$addColumnInfo(name = "variable",   title = "Variable",   type = "string", combine = TRUE)
    binomialTable$addColumnInfo(name = "level",      title = "Level",      type = "string")
    binomialTable$addColumnInfo(name = "counts",     title = "Counts",     type = "integer")
    binomialTable$addColumnInfo(name = "total",      title = "Total",      type = "integer")
    binomialTable$addColumnInfo(name = "proportion", title = "Proportion", type = "number", format = "sf:4")
    binomialTable$addColumnInfo(name = "p",          title = "p",          type = "number", format = "dp:3;p:.001")

    if (options$VovkSellkeMPR)
      binomialTable$addColumnInfo(name = "VovkSellkeMPR", title = "VS-MPR\u002A", type = "number", format = "sf:4")

    if (options$confidenceInterval) {
      binomialTable$addColumnInfo(name = "lowerCI", title = "Lower", type = "number", format = "sf:4",
        overtitle = paste0(100 * options$confidenceIntervalInterval, "% CI for Proportion"))
      binomialTable$addColumnInfo(name = "upperCI", title = "Upper", type = "number", format = "sf:4",
        overtitle = paste0(100 * options$confidenceIntervalInterval, "% CI for Proportion"))
    }
  ```
  
</details></p>

Another setting you may consider tweaking is whether JASP should display all columns it receives data for, or only the columns which were specified through `$addColumnInfo()`. This can be useful as results function are often blown up by complicated, nested if-statements in order to compute only the necessary statistics. By using `$showSpecifiedColumnsOnly`, we can avoid this complexity by computing the results for all statistics (even those that the user does not require). Note that the default behaviour is to show all columns for which data is received, unless `$showSpecifiedColumsnOnly` is set to `TRUE`.

<p><details>
	<summary>Code</summary>
  
  ```r
  .binomTableMain(jaspResults, dataset, options, ready) {
    binomialTable <- createJaspTable(title = "Binomial Test")

    binomialTable$dependOnOptions(c("variables", "testValue", "hypothesis", "confidenceInterval",
                                    "confidenceIntervalInterval", "VovkSellkeMPR"))
    
    binomialTable$addCitation("JASP Team (2018). JASP (Version 0.9.2) [Computer software].")
    
    binomialTable$addColumnInfo(name = "variable",   title = "Variable",   type = "string", combine = TRUE)
    binomialTable$addColumnInfo(name = "level",      title = "Level",      type = "string")
    binomialTable$addColumnInfo(name = "counts",     title = "Counts",     type = "integer")
    binomialTable$addColumnInfo(name = "total",      title = "Total",      type = "integer")
    binomialTable$addColumnInfo(name = "proportion", title = "Proportion", type = "number", format = "sf:4")
    binomialTable$addColumnInfo(name = "p",          title = "p",          type = "number", format = "dp:3;p:.001")

    if (options$VovkSellkeMPR)
      binomialTable$addColumnInfo(name = "VovkSellkeMPR", title = "VS-MPR\u002A", type = "number", format = "sf:4")

    if (options$confidenceInterval) {
      binomialTable$addColumnInfo(name = "lowerCI", title = "Lower", type = "number", format = "sf:4",
        overtitle = paste0(100 * options$confidenceIntervalInterval, "% CI for Proportion"))
      binomialTable$addColumnInfo(name = "upperCI", title = "Upper", type = "number", format = "sf:4",
        overtitle = paste0(100 * options$confidenceIntervalInterval, "% CI for Proportion"))
    }
    
    binomialTable$showSpecifiedColumnsOnly <- TRUE
  ```
 
 </details></p>

In the example given above, the column description added through `$addColumnInfo()` with `name = "VovkSellkeMPR"` will only be included when the VovkSellkeMPR checkbox in the interface is checked. By setting `$showSpecifiedColumnsOnly` to `TRUE` it does not matter if we include the VovkSellkeMPR statistic in our results anyway, as it won't be added to the table.

#### Step 6.1.5 - Expected Number of Rows
Optionally, we can tell JASP how many rows our table will have through `$setExpectedRows()`. In analyses that do not take a lot of time to run (i.e., their computations are quick) this is not really required. In this case, JASP will default to showing an empty table with a single row filled with dots until it receives your actual results. However, if your analysis is slow, it's recommended to create an empty table of the correct size and then fill this table row by row. The binomial test is quick, but we'll add it anyway:

<details>
	<summary>Code</summary>
  
  ```r
  .binomTableMain(jaspResults, dataset, options, ready) {
    binomialTable <- createJaspTable(title = "Binomial Test")

    binomialTable$dependOnOptions(c("variables", "testValue", "hypothesis", "confidenceInterval",
                                    "confidenceIntervalInterval", "VovkSellkeMPR"))
                                    
    binomialTable$addCitation("JASP Team (2018). JASP (Version 0.9.2) [Computer software].")    
    
    binomialTable$addColumnInfo(name = "variable",   title = "Variable",   type = "string", combine = TRUE)
    binomialTable$addColumnInfo(name = "level",      title = "Level",      type = "string")
    binomialTable$addColumnInfo(name = "counts",     title = "Counts",     type = "integer")
    binomialTable$addColumnInfo(name = "total",      title = "Total",      type = "integer")
    binomialTable$addColumnInfo(name = "proportion", title = "Proportion", type = "number", format = "sf:4")
    binomialTable$addColumnInfo(name = "p",          title = "p",          type = "number", format = "dp:3;p:.001")

    if (options$VovkSellkeMPR)
      binomialTable$addColumnInfo(name = "VovkSellkeMPR", title = "VS-MPR\u002A", type = "number", format = "sf:4")

    if (options$confidenceInterval) {
      binomialTable$addColumnInfo(name = "lowerCI", title = "Lower", type = "number", format = "sf:4",
        overtitle = paste0(100 * options$confidenceIntervalInterval, "% CI for Proportion"))
      binomialTable$addColumnInfo(name = "upperCI", title = "Upper", type = "number", format = "sf:4",
        overtitle = paste0(100 * options$confidenceIntervalInterval, "% CI for Proportion"))
    }
    
    binomialTable$showSpecifiedColumnsOnly <- TRUE
    
    if (ready)
      binomialTable$setExpectedRows(length(options$variables))
      
  ```
 
</details>

#### Step 6.1.6 - Adding Footnotes
If we wish to display a message about the table output we can do so with a footnote. This message will be displayed below the table and may refer to the table as a whole, or to a cell in specific. To add a footnote use `$addFootnote()` with the following arguments (\* denotes required arguments):
- `message`\*: string message we wish to display below the table
- `symbol`: if it is a general note about the entire table then use `"<em>Note.</em>"` , otherwise either omit it (in which case JASP will add a symbol automatically) or supply a unicode character (e.g., `\u207A`)
- `col_names`: string (vector) with column name(s) the footnote will point to (if used in conjunction with `row_names`, the footnote will point to the intersecting cell(s))
- `row_names`: string (vector) with row name(s) the footnote will point to (if used in conjunction with `col_names`, the footnote will point to the intersecting cell(s))

<details>
	<summary>Code</summary>
  
  ```r
  .binomTableMain(jaspResults, dataset, options, ready) {
    binomialTable <- createJaspTable(title = "Binomial Test")

    binomialTable$dependOnOptions(c("variables", "testValue", "hypothesis", "confidenceInterval",
                                    "confidenceIntervalInterval", "VovkSellkeMPR"))
    
    binomialTable$addCitation("JASP Team (2018). JASP (Version 0.9.2) [Computer software].")
        
    binomialTable$addColumnInfo(name = "variable",   title = "Variable",   type = "string", combine = TRUE)
    binomialTable$addColumnInfo(name = "level",      title = "Level",      type = "string")
    binomialTable$addColumnInfo(name = "counts",     title = "Counts",     type = "integer")
    binomialTable$addColumnInfo(name = "total",      title = "Total",      type = "integer")
    binomialTable$addColumnInfo(name = "proportion", title = "Proportion", type = "number", format = "sf:4")
    binomialTable$addColumnInfo(name = "p",          title = "p",          type = "number", format = "dp:3;p:.001")

    if (options$VovkSellkeMPR)
      binomialTable$addColumnInfo(name = "VovkSellkeMPR", title = "VS-MPR\u002A", type = "number", format = "sf:4")

    if (options$confidenceInterval) {
      binomialTable$addColumnInfo(name = "lowerCI", title = "Lower", type = "number", format = "sf:4",
        overtitle = paste0(100 * options$confidenceIntervalInterval, "% CI for Proportion"))
      binomialTable$addColumnInfo(name = "upperCI", title = "Upper", type = "number", format = "sf:4",
        overtitle = paste0(100 * options$confidenceIntervalInterval, "% CI for Proportion"))
    }
    
    binomialTable$showSpecifiedColumnsOnly <- TRUE
    
    if (ready)
      binomialTable$setExpectedRows(length(options$variables))
     
    message <- switch(options$hypothesis,
                      "notEqualToTestValue"  = paste0("Proportions tested against value: ", options$testValue, "."),
                      "greaterThanTestValue" = paste0("For all tests, the alternative hypothesis specifies that the proportion is greater than ", options$testValue, "."),
                      "lessThanTestValue"    = paste0("For all tests, the alternative hypothesis specifies that the proportion is less than ", options$testValue, ".")
    )
    binomialTable$addFootnote(message = message, symbol = "<em>Note.</em>")
  ```
 
</details>

#### Step 6.1.7 - Adding the Table to the Output
The markup part of the table is complete and we can now give it to `jaspResults` (and if we're not ready to compute anything we're done all together). `jaspResults` will automatically send updates of partial results as it receives them. Consequently, the user sees the output grow as you move through the analysis and add tables and plots:

<details>
	<summary>Code</summary>
  
  ```r
  .binomTableMain(jaspResults, dataset, options, ready) {
    binomialTable <- createJaspTable(title = "Binomial Test")

    binomialTable$dependOnOptions(c("variables", "testValue", "hypothesis", "confidenceInterval",
                                    "confidenceIntervalInterval", "VovkSellkeMPR"))
    
    binomialTable$addCitation("JASP Team (2018). JASP (Version 0.9.2) [Computer software].")
        
    binomialTable$addColumnInfo(name = "variable",   title = "Variable",   type = "string", combine = TRUE)
    binomialTable$addColumnInfo(name = "level",      title = "Level",      type = "string")
    binomialTable$addColumnInfo(name = "counts",     title = "Counts",     type = "integer")
    binomialTable$addColumnInfo(name = "total",      title = "Total",      type = "integer")
    binomialTable$addColumnInfo(name = "proportion", title = "Proportion", type = "number", format = "sf:4")
    binomialTable$addColumnInfo(name = "p",          title = "p",          type = "number", format = "dp:3;p:.001")

    if (options$VovkSellkeMPR)
      binomialTable$addColumnInfo(name = "VovkSellkeMPR", title = "VS-MPR\u002A", type = "number", format = "sf:4")

    if (options$confidenceInterval) {
      binomialTable$addColumnInfo(name = "lowerCI", title = "Lower", type = "number", format = "sf:4",
        overtitle = paste0(100 * options$confidenceIntervalInterval, "% CI for Proportion"))
      binomialTable$addColumnInfo(name = "upperCI", title = "Upper", type = "number", format = "sf:4",
        overtitle = paste0(100 * options$confidenceIntervalInterval, "% CI for Proportion"))
    }
    
    binomialTable$showSpecifiedColumnsOnly <- TRUE
    
    if (ready)
      binomialTable$setExpectedRows(length(options$variables))
     
    message <- switch(options$hypothesis,
                      "notEqualToTestValue"  = paste0("Proportions tested against value: ", options$testValue, "."),
                      "greaterThanTestValue" = paste0("For all tests, the alternative hypothesis specifies that the proportion is greater than ", options$testValue, "."),
                      "lessThanTestValue"    = paste0("For all tests, the alternative hypothesis specifies that the proportion is less than ", options$testValue, ".")
    )
    binomialTable$addFootnote(message = message, symbol = "<em>Note.</em>")
    
    jaspResults[["binomialTable"]] <- binomialTable
    
    if (!ready)
      return()
  ```
 
</details>


#### Step 6.1.8 - Filling the table
If we are indeed ready, then it is time to fill the table with computed results. This will be the final thing our table function does and we can return after we fill the table. Note that in this example we compute the results in the "fill function" although this is not strictly necessary. You can also create a dedicated function to compute results. Having separate functions is especially useful when multiple output elements make use of the same computed results. For more information on how to approach this see [ADDENDUM II - Reusing Results for Multiple Output Elements](#addendum-ii---reusing-results-for-multiple-output-elements).

<p><details>
	<summary>Code</summary>
  
  ```r
  .binomTableMain(jaspResults, dataset, options, ready) {
    binomialTable <- createJaspTable(title = "Binomial Test")

    binomialTable$dependOnOptions(c("variables", "testValue", "hypothesis", "confidenceInterval",
                                    "confidenceIntervalInterval", "VovkSellkeMPR"))
    
    binomialTable$addCitation("JASP Team (2018). JASP (Version 0.9.2) [Computer software].")
    
    binomialTable$addColumnInfo(name = "variable",   title = "Variable",   type = "string", combine = TRUE)
    binomialTable$addColumnInfo(name = "level",      title = "Level",      type = "string")
    binomialTable$addColumnInfo(name = "counts",     title = "Counts",     type = "integer")
    binomialTable$addColumnInfo(name = "total",      title = "Total",      type = "integer")
    binomialTable$addColumnInfo(name = "proportion", title = "Proportion", type = "number", format = "sf:4")
    binomialTable$addColumnInfo(name = "p",          title = "p",          type = "number", format = "dp:3;p:.001")

    if (options$VovkSellkeMPR)
      binomialTable$addColumnInfo(name = "VovkSellkeMPR", title = "VS-MPR\u002A", type = "number", format = "sf:4")

    if (options$confidenceInterval) {
      binomialTable$addColumnInfo(name = "lowerCI", title = "Lower", type = "number", format = "sf:4",
        overtitle = paste0(100 * options$confidenceIntervalInterval, "% CI for Proportion"))
      binomialTable$addColumnInfo(name = "upperCI", title = "Upper", type = "number", format = "sf:4",
        overtitle = paste0(100 * options$confidenceIntervalInterval, "% CI for Proportion"))
    }
    
    binomialTable$showSpecifiedColumnsOnly <- TRUE
    
    if (ready)
      binomialTable$setExpectedRows(length(options$variables))
     
    message <- switch(options$hypothesis,
                      "notEqualToTestValue"  = paste0("Proportions tested against value: ", options$testValue, "."),
                      "greaterThanTestValue" = paste0("For all tests, the alternative hypothesis specifies that the proportion is greater than ", options$testValue, "."),
                      "lessThanTestValue"    = paste0("For all tests, the alternative hypothesis specifies that the proportion is less than ", options$testValue, ".")
    )
    binomialTable$addFootnote(message = message, symbol = "<em>Note.</em>")
    
    jaspResults[["binomialTable"]] <- binomialTable
    
    if (!ready)
      return()
      
    .binomFillTableMain(binomialTable, dataset, options)
    
    return()
  }
  ```
 
</details></p>

Oftentimes, we need to add one row for each dependent variable or for each predictor. Therefore, the results are often added in for loops. In the example provided below, one row with results is added for each level of each dependent variable. The actual computations are not that important, so we'll limit what we show of `.binomFillTableMain` to the parts that are more generally applicable. It is important to note that we add these rows directly to the `binomialTable` we pass into the function and do not need to do anything further to `jaspResults`. The reason for this is that JASP tables are special objects that can use pass-by-reference mechanics. In this case `binomialTable` is passed by reference, which means that we are not just passing a copy of `binomialTable`, but an alias to the variable `binomialTable` from `.binomTableMain`. Consequently, any changes we make to `binomialTable` in `.binomFillTableMain` will be reflected by changes in `binomialTable` in `.binomTableMain`. And, as we added `binomialTable` to `jaspResults[["binomialTable"]]` earlier, all changes are automatically incorporated there, too. This means we do not need to return anything from the function and can simply exit with `NULL`.

<details>
	<summary>Code</summary>
  
  ```r
  .binomFillTableMain <- function(binomialTable, dataset, options) {

    for (variable in options$variables) {

      <calculate our results with stats::binom.test>

      binomialTable$addRows(variable      = variable,
                            level         = level,
                            counts        = counts,
                            total         = nObs,
                            proportion    = prop,
                            p             = p,
                            VovkSellkeMPR = vovkSellkeMPR,
                            lowerCI       = lowerCI,
                            upperCI       = upperCI
    }
    
    return()
  }
  ```
  
</details>

#### Step 6.1.9 - Reporting Errors
It's entirely possible that an analysis still crashes even after our error checking in [Step 5 - Checking for Errors](#step-5---checking-for-errors). There are two things we can about this. One, nothing at all. If the analysis crashes it will crash hard and 'the analysis terminated unexpectedly' will be shown. This would be the situation in step 6.1.8. On the other hand, we could also still try to compute other results if possible (i.e., there might be a different part of the analysis which could still be computed) and then exit the analysis normally. To accomplish the second situation you will have to use R's `try()` and combine it with JASP's `$setError()`.

<details>
	<summary>Code</summary>
  
  ```r
  .binomFillTableMain <- function(binomialTable, dataset, options) {

    for (variable in options$variables) {

      results <- try(<calculate our results with stats::binom.test>)
      
      if (inherits(results, "try-error") {
        errorMessage <- as.character(results)
        binomialTable$setError(errorMessage)
        return()
      }
        
      binomialTable$addRows(variable      = results$variable,
                            level         = results$level,
                            counts        = results$counts,
                            total         = results$nObs,
                            proportion    = results$prop,
                            p             = results$p,
                            VovkSellkeMPR = results$vovkSellkeMPR,
                            lowerCI       = results$lowerCI,
                            upperCI       = results$upperCI
    }
    
    return()
  }
  ```
  
</details>

#### Step 6.1.8 - Multiple Tables
You might wish to add more tables to your analysis in which case you can simply repeat the steps above. Of course, it is possible that you have multiple tables that all need the same computed results and you do not wish to compute these again, this situation is described in [ADDENDUM II - Reusing Results for Multiple Output Elements](#addendum-ii---reusing-results-for-multiple-output-elements). Similarly, you might wish to visually group multiple output elements together, the steps needed to accomplish this are described in [ADDENDUM I - Grouping Multiple Output Elements Together](#addendum-i---grouping-multiple-output-elements-together).

### Step 6.2 - Plots
We have one table so far, let's create a plot, too. This will be the final output from our analysis, so we can return once we've created it.

<p><details>
	<summary>Code</summary>
	
  ```r
  BinomialTest <- function(jaspResults, dataset, options) {

    jaspResults$title <- "Binomial Test"
    
    ready <- (length(options$variables) > 0)
    
    if (ready) {
      dataset <- .binomReadData(dataset, options)
      
      .binomCheckErrors(dataset, options, ready)
    }
    
    if (is.null(jaspResults[["binomialTable"]])
      .binomTableMain(jaspResults, dataset, options, ready)
      
    if (is.null(jaspResults[["binomialPlot"]])
      .binomPlotDescriptives(jaspResults, dataset, options, ready)
      
    return()
  }
  ```

</details></p>

The origin of the name `"binomialPlot"` will become apparent in [Step 6.2.5 - Adding the Plot to the Output](#step-625---adding-the-plot-to-the-output).

#### Step 6.2.1 - Creating a JASP Plot
A JASP plot requires fairly little markup. All we have to do at the start is create a JASP plot object with a title, a width and height (in pixels):

<details>
	<summary>Code</summary>
  
  ```r
  .binomPlotDescriptives <- function(jaspResults, dataset, options, ready) {
    binomialPlot <- createJaspPlot(title = "Descriptives",  width = 160, height = 320)
    
  ```
  
</details>

#### Step 6.2.2 - Dependencies
Now we need to specify the analysis options on which the plot depends. If the value of any of these options changes, the plot will be removed from the output and needs to be newly computed. However, if none of these options changes the next time the analysis is called, the plot can be reused:

<details>
	<summary>Code</summary>
  
  ```r
  .binomPlotDescriptives <- function(jaspResults, dataset, options, ready) {
    binomialPlot <- createJaspPlot(title = "Descriptives",  width = 160, height = 320)
    
    binomialPlot$dependOnOptions(c("variables", "testValue", "descriptivesPlotsConfidenceInterval"))
  ```
  
</details>

#### Step 6.2.3 - Citations
Many analyses in JASP are based on the work of others and it is important we give them credit. To add citations to a JASP plot you can use `$addCitation()`; if no citation is given it defaults to the JASP Team.

<details>
	<summary>Code</summary>
  
  ```r
  .binomPlotDescriptives <- function(jaspResults, dataset, options, ready) {
    binomialPlot <- createJaspPlot(title = "Descriptives",  width = 160, height = 320)
    
    binomialPlot$dependOnOptions(c("variables", "testValue", "descriptivesPlotsConfidenceInterval"))
				    
    binomialPlot$addCitation("JASP Team (2018). JASP (Version 0.9.2) [Computer software].")
  ```

</details>

#### Step 6.2.4 - Adding Footnotes
If you wish to display a message below the plot you can do so with a footnote. To add a footnote use `$addFootnote()` with the following arguments (\* denotes required arguments):
- `message`\*: string message we wish to display below the plot
- `symbol`: in most cases should be set to `"<em>Note.</em>"`, considering no specific part of a plot can be targeted in the same way a table cell can be targeted

<details>
	<summary>Code</summary>
  
  ```r
  .binomPlotDescriptives <- function(jaspResults, dataset, options, ready) {
    binomialPlot <- createJaspPlot(title = "Descriptives",  width = 160, height = 320)
    
    binomialPlot$dependOnOptions(c("variables", "testValue", "descriptivesPlotsConfidenceInterval"))
				    
    binomialPlot$addCitation("JASP Team (2018). JASP (Version 0.9.2) [Computer software].")
    
    binomialPlot$addFootnote(message="Some descriptive text we want to show!", symbol="<em>Note.</em>")
  ```

</details>

#### Step 6.2.5 - Adding the Plot to the Output
We can now give it to `jaspResults` (and if we're not ready to compute anything we're done all together). When JASP receives a JASP plot object without an actual plot (i.e., ggplot) it will automatically show an empty plot of the correct size in the output.

<details>
	<summary>Code</summary>
  
  ```r
  .binomPlotDescriptives <- function(jaspResults, dataset, options, ready) {
    binomialPlot <- createJaspPlot(title = "Descriptives",  width = 160, height = 320)
    
    binomialPlot$dependOnOptions(c("variables", "testValue", "descriptivesPlotsConfidenceInterval"))
    				    
    binomialPlot$addCitation("JASP Team (2018). JASP (Version 0.9.2) [Computer software].")
    
    binomialPlot$addFootnote(message="Some descriptive text we want to show!", symbol="<em>Note.</em>")
    
    jaspResults[["binomialPlot"]] <- binomialPlot
    
    if (!ready)
      return()
  ```
  
</details>

#### Step 6.2.6 - Filling the Plot
If we are indeed ready, then it is time to create a ggplot object. This will be the final thing our plot function does and we can return after we created the plot.

<p><details>
	<summary>Code</summary>
  
  ```r
  .binomPlotDescriptives <- function(jaspResults, dataset, options, ready) {
    binomialPlot <- createJaspPlot(title = "Descriptives",  width = 160, height = 320)
    
    binomialPlot$dependOnOptions(c("variables", "testValue", "descriptivesPlotsConfidenceInterval"))
    				    
    binomialPlot$addCitation("JASP Team (2018). JASP (Version 0.9.2) [Computer software].")
    
    binomialPlot$addFootnote(message="Some descriptive text we want to show!", symbol="<em>Note.</em>")
    
    jaspResults[["binomialPlot"]] <- binomialPlot
    
    if (!ready)
      return()
      
    .binomFillPlotDescriptives(binomialPlot, dataset, options)
  
    return()
  }
  ```
  
</details></p>

Just like we did with the table earlier, we can use the pass-by-reference mechanics of `binomialPlot` to avoid having to return anything from our plotting function. Instead we just set the `plotObject` property of `binomialPlot` to a ggplot object. JASP does not support any other plotting method than ggplot, as we wish to create a uniform plot editing solution in the future.

<details>
	<summary>Code</summary>
  
  ```r
  .binomFillPlotDescriptives <- function(binomialPlot, dataset, options)
   
    plot <- ggplot2::ggplot(<code>)
   
    binomialPlot$plotObject <- plot
   
    return()
  }
  ```
  
</details>

#### Step 6.2.7 - Reporting Errors
It's entirely possible that an analysis still crashes even after our error checking in [Step 5 - Checking for Errors](#step-5---checking-for-errors). There are two things we can about this. One, nothing at all. If the analysis crashes it will crash hard and 'the analysis terminated unexpectedly' will be shown. This would be the situation in step 6.2.6. On the other hand, we could also still try to compute other results if possible (i.e., there might be a different part of the analysis which could still be computed) and then exit the analysis normally. To accomplish the second situation you will have to use R's `try()` and combine it with JASP's `$setError()`.

<details>
	<summary>Code</summary>
  
  ```r
  .binomFillPlotDescriptives <- function(binomialPlot, dataset, options)
   
    plot <- try(ggplot2::ggplot(<code>))
    
    if (inherits(plot, "try-error") {
      errorMessage <- as.character(plot)
      binomialPlot$setError(errorMessage)
      return()
    }
    
    binomialPlot$plotObject <- plot
   
    return()
  }
  ```
  
</details>

#### Step 6.2.8 - Multiple Plots
You might wish to add more plots to your analysis in which case you can simply repeat the steps above. Of course, it is possible that you have multiple plots that all need the same computed results and you do not wish to compute these again, this situation is described in [ADDENDUM II - Reusing Results for Multiple Output Elements](#addendum-ii---reusing-results-for-multiple-output-elements). Similarly, you might wish to visually group multiple output elements together, the steps needed to accomplish this are described in [ADDENDUM I - Grouping Multiple Output Elements Together](#addendum-i---grouping-multiple-output-elements-together).


ADDENDUM I - Grouping Multiple Output Elements Together
------------------------------------------------------

It is possible to create visually and functionally distinct groupings in your analysis. An example of a visual grouping can be seen in the following image, where "Descriptives Plots" and "facGender" are both groupings:

![Image Visual Grouping](/Docs/development/img/r-guide/example_visual_grouping.png)

What we mean by functional grouping is a set of tables and plots that all rely on the same dependencies and/or computed results. You may not wish to create a visual grouping, but you might still want to make use of the shared properties of the different output elements. Whether you wish to create a visual and/or functional grouping does not matter terribly much, they both make use of the same mechanic: JASP containers. For all intends and purposes a container can be thought of as a normal list in R, you can add objects to it and then retrieve them again at a later point.

### Creating a JASP Container
It's simple to create a container through `createJaspContainer()`:

<p><details>
	<summary>Code</summary>
  
  ```r
  modelContainer <- createJaspContainer(title = "Some Title")
  jaspResults[["modelContainer"]] <- modelContainer
  ```
  
</details></p>

Note that when we omit the title argument we do not get a visually distinct grouping in our output, but instead only make a functional grouping.

We can nest as many containers as we want:

<details>
	<summary>Code</summary>
  
  ```r
  modelContainer <- createJaspContainer(title = "Some Title")
  deeperModelContainer <- createJaspContainer(title = "Some Title Too")
  jaspResults[["modelContainer"]] <- modelContainer
  
  # we can add the container like this:
  modelContainer[["deeperModelContainer"]] <- deeperModelContainer
  
  # or this:
  modelContainer <- deeperModelContainer
  ```
  
</details>

### Using a Container
It's fairly straightforward to make use of a container as it doesn't change the way we insert a table or a plot.

<details>
	<summary>Code</summary>
  
  ```r
  mixedContainer <- createJaspContainer(title = "Collection of a Table and a Plot")
  jaspResults[["mixedContainer"]] <- mixedContainer
  
  plot <- createJaspPlot(title = "Plot",  width = 320, height = 320)
  table <- createJaspTable(title = "Table")
  
  mixedContainer[["plot"]] <- plot
  # or
  jaspResults[["mixedContainer"]][["table"]] <- table
  ```
  
</details>

### Dependencies
The neat thing about a container is that the dependencies we set on it are implicitly added to its contents. So instead of setting the same dependencies on all the tables and plots in the container, we only have to add them once on the container itself. And so when a relevant option changes, the container (and all its contents) is removed.

<p><details>
	<summary>Code</summary>
  
  ```r
  mixedContainer <- createJaspContainer(title = "Collection of a Table and a Plot")
  mixedContainer$dependOnOptions <- c("opt1", "opt2", "opt3")
  jaspResults[["mixedContainer"]] <- mixedContainer
  
  plot <- createJaspPlot(title = "Plot",  width = 320, height = 320)
  table <- createJaspTable(title = "Table")
  
  mixedContainer[["plot"]] <- plot
  mixedContainer[["table"]] <- table
  ```
  
</details></p>

Of course we can still also set dependencies on the tables and plots in the container:

<p><details>
	<summary>Code</summary>
  
  ```r
  mixedContainer <- createJaspContainer(title = "Collection of a Table and a Plot")
  mixedContainer$dependOnOptions("opt1", "opt2", "opt3")
  jaspResults[["mixedContainer"]] <- mixedContainer
  
  plot <- createJaspPlot(title = "Plot",  width = 320, height = 320)
  plot$dependOnOptions("opt4")
  table <- createJaspTable(title = "Table")
  table$dependOnOptions("opt5")
  
  mixedContainer[["plot"]] <- plot
  mixedContainer[["table"]] <- table
  ```
  
</details></p>

In the above example, if the value changes of `"opt1"`, `"opt2"` or `"opt3"` then the container is discarded and as a result so are both the table and plot. However, if only `"opt4"` changes in value, then both the container and the table can be reused.

#### Value Remains in Option
We can go one step further still. We can tell JASP to look at the actual values of an option. To exemplify what we mean, suppose we have code that generates descriptive plots of all the variables a user assigns to `independentVariables`. The user decides to add four variables to `independentVariables` and we plot them. Now, if the user decides to remove one of the variables, objectively `independentVariables` changes in value and, if specified through `$dependOnOptions()`, it should be discarded. This would be quite wasteful, as all plots were already generated; we only need to remove the plot of the variable that the user removed. To accomplish this, we can tell JASP that an output element must be kept while a specific value of an option remains in the option. For example, we would like to keep the descriptives plot for `variable1` as long as `variable1` is assigned to `independentVariables`. We can may do this through the method `$setOptionMustContainDependency()`:

<details>
	<summary>Code</summary>
  
  ```r
  descriptivePlotContainer <- createJaspContainer(title = "Descriptive Plots")
  descriptivePlotContainer$dependOnOptions(c("confidenceInterval", "plotDescriptives"))
  jaspResults[["descriptivePlotContainer"]] <- descriptivePlotContainer
  
  # create empty plots in the output
  for (variable in options$independentVariables) {
    # check if the plot for this variable was already created previously
    if (!is.null(descriptivePlotContainer[[variable]]))
      next
    plot <- createJaspPlot(title = variable,  width = 480, height = 320)
    plot$setOptionMustContainDependency("independentVariables", variable)
    descriptivePlotContainer[[variable]] <- plot
  }
  
  <now we can start adding actual plot objects given that they do not yet exist>
  ```
  
</details>

### Propagating Errors Through a Container
Suppose we are in the process of adding multiple output elements to a container, but encounter an error early on. We know that the remaining tables and plots cannot be filled with results, but they must be told as much. We could create an error object and pass this along between the functions, but this is a little cumbersome. Instead, we can set the error on the container and then all the contents of this container will receive an error status automatically. All we have to do at that point is check if our container's `$getError()` returns `TRUE` and if so only display empty tables and plots. The error message will automatically be shown on top of the first output element of the container.

<details>
	<summary>Code</summary>
  
  ```r
  mixedContainer <- createJaspContainer(title = "Collection of a Table and a Plot")
  jaspResults[["mixedContainer"]] <- mixedContainer
  
  mixedContainer$setError("This is an error message")
  
  plot <- createJaspPlot(title = "Plot",  width = 320, height = 320)
  table <- createJaspTable(title = "Table")
  
  mixedContainer[["plot"]] <- plot
  mixedContainer[["table"]] <- table
  
  if (mixedContainer$getError())
    return()
    
  <.fillInPlot(plot)>
  <.fillInTable(table)>
  ```
  
</details>


ADDENDUM II - Reusing Results for Multiple Output Elements
---------------------------------------------------------
There are multiple approaches to the way you can write an analysis (e.g., computing statistics in a different function than where they are used to fill a table/plot). This really does not matter and comes down to preference. However, in some situations it's much better to implement your analysis in a certain way. In this section we will cover one of those situations: computing results once and then using those results for multiple tables and plots. To exemplify this situation we will use code snippets from the Binomial Test we created above. Recall that in [Step 6.1.8 - Filling the table](#step-618---filling-the-table) we showed how you may calculate results and then add these results as rows of a JASP table. Our first action will be to split computing statistics from filling the table -- computing results should happen in its own dedicated function.

<p><details>
	<summary>Code</summary>
  
  ```r
  .binomComputeResults <- function(dataset, options) {
  
    results <- list()
    for (variable in options$variables) {

      <calculate our results with stats::binom.test>
      
      results[[variable]] <- list(level         = level,
                                  counts        = counts,
                                  total         = nObs,
                                  proportion    = prop,
                                  p             = p,
                                  VovkSellkeMPR = vovkSellkeMPR,
                                  lowerCI       = lowerCI,
                                  upperCI       = upperCI
    }
    
    return(results)
  }
  ```
  
</details></p>

However, this approach poses two problems, (1) there is no way to store these results and avoid performing the computations every time our analysis is run. And (2) we have no efficient way to pass the results object around between the functions that use the results to fill tables and plots. 

### Creating a JASP State
In a JASP state we may store any R object and all the stored R objects will be available during subsequent runs of the analysis, given that the dependencies have not changed. They are hidden from the user and only available to the analysis. Creating a JASP state is much like creating a JASP container:

<details>
	<summary>Code</summary>
  
  ```r
  .binomComputeResults <- function(jaspResults, dataset, options) {
    
    binomResults <- createJaspState()
    jaspResults[["binomResults]] <- binomResults
  ```
  
</details>

### Dependencies
Now we need to specify the analysis options on which the state object will depend. If the value of any of these options changes, the state object will be removed and needs to be newly computed. However, if none of these options changes the next time the analysis is called, the state object can be reused:

<details>
	<summary>Code</summary>
  
  ```r
  .binomComputeResults <- function(jaspResults, dataset, options) {
    
    binomResults <- createJaspState()
    jaspResults[["binomResults]] <- binomResults
    binomResults$dependOnOptions(c("variables", "testValue", "hypothesis", "confidenceIntervalInterval"))
  ```
  
</details>

### Computing Results
Time to compute results that we will be able to store in the state:

<details>
	<summary>Code</summary>
  
  ```r
  .binomComputeResults <- function(jaspResults, dataset, options) {
    
    binomResults <- createJaspState()
    jaspResults[["binomResults]] <- binomResults
    binomResults$dependOnOptions(c("variables", "testValue", "hypothesis", "confidenceIntervalInterval"))
    
    results <- list()
    for (variable in options$variables) {

      <calculate our results with stats::binom.test>
      
      results[[variable]] <- list(level         = level,
                                  counts        = counts,
                                  total         = nObs,
                                  proportion    = prop,
                                  p             = p,
                                  VovkSellkeMPR = vovkSellkeMPR,
                                  lowerCI       = lowerCI,
                                  upperCI       = upperCI
    } # end for-loop
  ```
  
</details>

### Storing Results in the State
We computed our results and are ready to use them to fill any table and plot that requires them. But first we must store them in the JASP state `$object` so we may fetch them at any point during this analysis or a subsequent run.

<details>
	<summary>Code</summary>
  
  ```r
  .binomComputeResults <- function(jaspResults, dataset, options) {
    
    binomResults <- createJaspState()
    jaspResults[["binomResults]] <- binomResults
    binomResults$dependOnOptions(c("variables", "testValue", "hypothesis", "confidenceIntervalInterval"))
    
    results <- list()
    for (variable in options$variables) {

      <calculate our results with stats::binom.test>
      
      results[[variable]] <- list(level         = level,
                                  counts        = counts,
                                  total         = nObs,
                                  proportion    = prop,
                                  p             = p,
                                  VovkSellkeMPR = vovkSellkeMPR,
                                  lowerCI       = lowerCI,
                                  upperCI       = upperCI
    }
    
    binomResults$object <- results
    
    return()
  }
  ```
  
</details>

### Retrieving the State Object
At this point we have the functionality to compute the binomial results whenever we need them, in whichever table or plot. Furthermore, the results may be retrieved directly from `jaspResults` without us having to pass it around. To exemplify how you could approach the retrieval process of these results, we'll rewrite [Step 6.1.8 - Filling the table](#step-618---filling-the-table) to make use of `.binomComputeResults()`. Note that the changes can found in the last five statements.

<p><details>
	<summary>Code</summary>
  
  ```r
  .binomTableMain(jaspResults, dataset, options, ready) {
    binomialTable <- createJaspTable(title = "Binomial Test")

    binomialTable$dependOnOptions(c("variables", "testValue", "hypothesis", "confidenceInterval",
                                    "confidenceIntervalInterval", "VovkSellkeMPR"))
    
    binomialTable$addCitation("JASP Team (2018). JASP (Version 0.9.2) [Computer software].")
    
    binomialTable$addColumnInfo(name = "variable",   title = "Variable",   type = "string", combine = TRUE)
    binomialTable$addColumnInfo(name = "level",      title = "Level",      type = "string")
    binomialTable$addColumnInfo(name = "counts",     title = "Counts",     type = "integer")
    binomialTable$addColumnInfo(name = "total",      title = "Total",      type = "integer")
    binomialTable$addColumnInfo(name = "proportion", title = "Proportion", type = "number", format = "sf:4")
    binomialTable$addColumnInfo(name = "p",          title = "p",          type = "number", format = "dp:3;p:.001")

    if (options$VovkSellkeMPR)
      binomialTable$addColumnInfo(name = "VovkSellkeMPR", title = "VS-MPR\u002A", type = "number", format = "sf:4")

    if (options$confidenceInterval) {
      binomialTable$addColumnInfo(name = "lowerCI", title = "Lower", type = "number", format = "sf:4",
        overtitle = paste0(100 * options$confidenceIntervalInterval, "% CI for Proportion"))
      binomialTable$addColumnInfo(name = "upperCI", title = "Upper", type = "number", format = "sf:4",
        overtitle = paste0(100 * options$confidenceIntervalInterval, "% CI for Proportion"))
    }
    
    binomialTable$showSpecifiedColumnsOnly <- TRUE
    
    if (ready)
      binomialTable$setExpectedRows(length(options$variables))
     
    jaspResults[["binomialTable"]] <- binomialTable
    
    if (!ready)
      return()
      
    if (is.null(jaspResults[["binomResults"]])
      .binomComputeResults(jaspResults, dataset, options)
    
    binomResults <- jaspResults[["binomResults"]]$object
    
    .binomFillTableMain(binomialTable, binomResults)
    
    return()
  }
  ```
 
</details></p>

And now we can call `.binomFillTableMain()` with the added `binomResults` argument (`dataset` and `options` are no longer needed):

<details>
	<summary>Code</summary>
  
  ```r
  .binomFillTableMain <- function(binomialTable, binomResults) {

    for (variable in options$variables) {
      results <- binomResults[[variable]]
      
      binomialTable$addRows(variable      = variable,
                            level         = results$level,
                            counts        = results$counts,
                            total         = results$nObs,
                            proportion    = results$prop,
                            p             = results$p,
                            VovkSellkeMPR = results$vovkSellkeMPR,
                            lowerCI       = results$lowerCI,
                            upperCI       = results$upperCI
    }
    
    return()
  }
  ```
  
</details>

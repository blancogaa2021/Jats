Git Guide
=========

Git ([website](https://git-scm.com/), [Wiki](https://en.wikipedia.org/wiki/Git)) is a version control system that the JASP Team relies upon to manage different versions of JASP throghout its development. This document gives tips for working with `git` an GitHub, specifically in the context of the JASP module development workflow. 

## Prerequisites


This guide assumes you have `git` installed [(instructions)](https://git-scm.com/book/en/v2/Getting-Started-Installing-Git) and that you have an account on `GitHub` [(make an account)](https://github.com/join). 

For Windows users: The standard recommendation for using `git` is by installing [Git BASH](https://gitforwindows.org/).

### Git

We strongly recommend getting to know using `git` itself instead of relying on clients such as GiHub Desktop, GitKraken, etc (unless you know what you are doing). If you follow this guide, it is less likely that you will get into trouble than if you use a client. Further, should something go wrong anyway, it is then easier for us to help you fix it. As with any programming gimmick, read carefully what `git` tells you, and "google up" messages you do not undestand. In general, internet is your friend and the first one to give you good answers about `git`. A useful website is the official GitHub documentation [https://docs.github.com/](https://docs.github.com/), and there also exist useful video tutorials ([example](https://youtu.be/8Dd7KRpKeaE)). Most of the information presented in this guide can be already found in the `git` and GitHub documentation as well, but this guide is describing the workflow in context of JASP development to be more explicit.

### The terminal

Because we do not recommend any `git` client, the following guide assumes basic understanding of a terminal [(tutorial here)](https://code.tutsplus.com/tutorials/command-line-basics-and-useful-tricks-with-the-terminal--cms-29356), but one does not need to be very experienced with it. Search and click on the `Terminal`/`Console` application in MacOS/Linux, or one of the terminal applications in Windows (e.g., `PowerShell`, `Command Prompt`, `Git BASH`) to get started. The only really important thing to know about the terminal is that one can type a command inside of the terminal and execute it with pressing `Enter` to tell the terminal to do something. 

In this guide, the only really important activity with the terminal is to be able to navigate between different folders in your computer, and running `git`. Whenever this guide says something like "navigate your terminal to a folder `jaspRegression`", this means to change the working directory of the terminal such that we are inside the folder called `jaspRegression`. The following commands will help that task done:

- `pwd`: Executing this command prints the current working directory of the terminal
- `ls`: Executing this command prints the contents of the current working directory (i.e., prints the files and folders)
- `tree`: Executing this command prints the contents of the current working directory in a structural manner, also showing the contents of folders inside the working folder, etc. For example, executing `tree -L 2` shows the contents of the working directory recursively up to two levels, e.g., shows contents of the folders inside of the working directory.
- `mkdir`: Executing this command coupled with a path creates a folder (directory) at the specified location. For example, `mkdir myNewFolder` creates a folder called `myNewFolder` inside of the current working directory.
- `cd`: Executing this command changes the folder of the terminal, based on the address that follows the `cd` keyword. For example:
  - `cd jaspRegression`: Moves the terminal inside of the `jaspRegression` folder (as long as it exist within the current working directory). This is an example of a *relative* path: Writing just the name of the path you want to go to assumes that the starting point is wherever the current working directory is.
  - `cd ~/Dektop/JASP/Development/Modules/jaspRegression`: Moves the terminal into the folder specified on that address. This is an example of an *absolute* path: Writing `~` in front of the path  you want to go assumes that the starting point is the user root directory. Executing `cd` without anything moves the terminal to that user root directorys.
  - `cd ../../`: Two dots `..` tell `cd` to move one level up in the folder structure. For example, if we execute `cd ~/Dektop/JASP/Development/Modules/jaspRegression` and then `cd ../../`, we would end up in the `~/Dektop/JASP/Development/` folder.

For Windows users: Paths in Windows are specified using a backslash `\` instead of a forward slash `\`, and instead of using `~/` for the user root folder, absolute paths are specified by starting from the drive name, e.g., `C:\JASP\jaspRegression`.

To run `git` in the terminal, we execute `git` commands - these start with typing `git` and follow some other keywords or arguments that specify what exactly we want `git` to do. For example, executing `git status` shows the status of the current `git` repository.


See [various cheatsheets](https://www.codecademy.com/resources/cheatsheets/language/bash) designed to help with the basic terminal and git commands.

### Customization

The terminal, as well as git, can be customised to make it nicer for you to work with it. For example, see a customization framework for the popular `zsh` shell, [Oh My Zsh](https://ohmyz.sh/).

At some points, `git` may require a text editor to complete some command. In those cases, it automatically opens the terminal text editor that is configured for the terminal. For many people, this will be `Vim` ([website](https://www.vim.org/), [Wiki](https://en.wikipedia.org/wiki/Vim_(text_editor))). Some people prefer other editors, for example `Nano` ([website](https://www.nano-editor.org/), [Wiki](https://en.wikipedia.org/wiki/GNU_nano)). To set `Nano` as the default text editor, you need to open the terminal configuration file (`.bashrc` or `.zshrc`) and add a line `EDITOR=nano`. In Windows, text editor settings should be available during installation of `Git BASH`.


## JASP module development workflow [(GitHub documentation)](https://docs.github.com/en/get-started/quickstart/github-flow)

Each JASP module is its own GitHub repository hosted on [the JASP Statistics Project](https://github.com/jasp-stats) (henceforth referred to as `jasp-stats`). For example, the "Regression" module lives as a main repository at `jasp-stats`: [https://github.com/jasp-stats/jaspRegression](https://github.com/jasp-stats/jaspRegression). Everyone can contribute to any JASP module by changing the code in the repository associated with the module. However, in order to keep the `jasp-stats` repository clean, not everyone is permitted to make changes directly to the `jasp-stats` repository. 

Instead, each contributor is required to keep their own "fork" (i.e., a copy) of the module repository, hosted on their own GitHub account, which they can modify as much as they like. Once a contributor is happy with the changes and wants them to appear in JASP itself, they make a "Pull Request" (henceforth: PR) to `jasp-stats`, which is like saying: "Hi, here are my proposed changes to the module, do you want to add them to the `jasp-stats` repository?". Someone from the JASP Team will review the changes, and if they approve the PR, the code is then "merged", meaning that the changes were added to the `jasp-stats` repository and the history of the project was updated to hold information about the new contribution.

In a nutshell: Each contributor keeps their own fork of the `jasp-stats`, maintain their own fork of the repository (i.e., keep it up to date with `jasp-stats` and adding new features), and through PRs propose changes to the `jasp-stats` repository. 

The following guide will use the "Regression" module as a concrete example. Keyword `username` will be used as a placeholder for your username on GitHub.

Note: Arguably the most important repository in `jasp-stats`, but also the one that you probably will not work with very often (if you are an R contributor or translator), is the `jasp-desktop` repository at [https://github.com/jasp-stats/jasp-desktop](https://github.com/jasp-stats/jasp-desktop). This repository holds the core code that is required to build the JASP application. Working with this repository requires additional steps, which are described in section [Git submodules: `jasp-stats/jasp-desktop`](#git-submodules-jasp-statsjasp-desktop).

## Setting up your repository

These steps need to be done only once per module. When everything it set up, you are ready to contribute to JASP!

### Fork the `jasp-stats` repository [(GitHub documentation)](https://docs.github.com/en/get-started/quickstart/fork-a-repo)

Navigate to the repository containing the module you want to fork (copy), e.g., [https://github.com/jasp-stats/jaspRegression](https://github.com/jasp-stats/jaspRegression). On the top-right, click on `Fork` button and confirm. A fork of the module was created under your GitHub account, and you can access it at `https://github.com/username/jaspRegression`. Congratulations, you have your own copy of the module!

### Clone your repository [(GitHub documentation)](https://docs.github.com/en/get-started/quickstart/fork-a-repo#cloning-your-forked-repository)

To be able to work on the code on your computer, you need to clone the repository to make a local copy on your machine. Navigate your browser to your module repository (e.g., `https://github.com/username/jaspRegression`), click on `Code` button, and copy the address under the "Clone" heading. The adress would look something like `https://github.com/username/jaspRegression.git`. Open the terminal (or git-bash on Windows) on your computer, navigate to the folder where you want to have the repository on your computer (using `cd`, [https://en.wikipedia.org/wiki/Cd_(command)](https://en.wikipedia.org/wiki/Cd_(command))) and then type

```
git clone https://github.com/username/jaspRegression.git
```

Now, the folder in which you called this command will contain another folder, `jaspRegression/`, which contains the local copy of the repository. Execute `cd jaspRegression` in the terminal to jump inside the repository.

### Connect your local clone to `jasp-stats` [(GitHub documentation)](https://docs.github.com/en/get-started/quickstart/fork-a-repo#configuring-git-to-sync-your-fork-with-the-original-repository)

In order to be able to keep your repository in sync with the one under `jasp-stats`, you need to specify the `jasp-stats` repository as a new "remote", essentially making it possible to fetch updates from `jasp-stats` into your local clone whenever you ask for it.

Navigate your browser to the `jasp-stats` repo, and copy the address as in the previous point; the address would look something like `https://github.com/jasp-stats/jaspRegression.git`.

Open the terminal (or git-bash on Windows), navigate to the local clone on your computer, and type

```
git remote add upstream https://github.com/jasp-stats/jaspRegression.git
```

to confirm that the repository has been added, type `git remote -v`. Your repository should show up in the terminal as `origin`, the `jasp-stats` as `upstream`, like this:

```
origin    https://github.com/username/jaspRegression.git (fetch)
origin    https://github.com/username/jaspRegression.git (push)
upstream  https://github.com/jasp-stats/jaspRegression.git (fetch)
upstream  https://github.com/jasp-stats/jaspRegression.git (push)
```

If that is the case, great! Your local clone of the repository stored on your computer is now able to communicate both with your personal fork of the `jaspRegression` module under your GitHub account, as well as the one stored under the `jasp-stats` project.

## Rebasing your repository [(GitHub documentation)](https://docs.github.com/en/get-started/using-git/about-git-rebase)

To get updates from `jasp-stats` into your local repository and your GitHub repository, you will need to do a rebase. It is good idea to rebase often, and you should **always** rebase before making a PR.

To do a rebase, navigate your terminal to the location of your local clone. Then, type in

1. `git stash` - optional: this command stashes (temporarily hides) changes that you made in the repository, but did not commit. Generally, you do not need to do this if you are sure you did not modify any files in the repository without committing the changes.
2. `git fetch upstream` - this command gets the code from the `jasp-stats` repository (if you set up your remote correctly, see above). The code from `jasp-stats` repository is not yet *applied* to your repository, but is available on your machine.
3. `git rebase upstream/master` - this command updates your local repository: the history of the repository on your computer becomes up-to-date with the repository on `jasp-stats` and its `master` branch.
4. `git push` - this command pushes the update to your remote repository on GitHub: now your local clone, as well as your GitHub repository, are up to date with `jasp-stats`.
5. `git stash pop` - optional: recovers the changes that you stashed before (**do not** call this command if you did not call `git stash` in the first place, or if `git stash` returned "No local changes to save")

### Merge conflicts during rebase? [(GitHub documentation)](https://docs.github.com/en/get-started/using-git/resolving-merge-conflicts-after-a-git-rebase)

In case that rebasing fails with git telling you it could not apply some commits, it means that you made some commits to your current project that are not compatible with the updates on `jasp-stats` (not compatible as in both versions attempt to make changes in the same lines of code). The conflict can range from trivial to complex, and so can the resolution. Follow the instructions for resolving merge conflicts [(GitHub documentation)](https://docs.github.com/en/pull-requests/collaborating-with-pull-requests/addressing-merge-conflicts/resolving-a-merge-conflict-using-the-command-line), or seek an advice from other JASP contributor. 

Remember that the best solution of merge conflicts is to avoid them at all, and that involves rebasing your repository often to make sure you are always up to date. Even if there eventually come some conflict, rebasing often can reduce a chance that the size of the conflicts are large, which leads to easier resolution.

## Contributing to `jasp-stats`

### Implement your changes

#### Make a feature branch [(GitHub documentation)](https://docs.github.com/en/pull-requests/collaborating-with-pull-requests/proposing-changes-to-your-work-with-pull-requests/about-branches)

The `master` branch on the `jasp-stats` repository is the default branch, which means that the default branch on your own repository is also `master` branch. Your `master` branch should be kept up to date with `jasp-stats/master`, and you should not work on this branch. Making a PR from the `master` branch is considered an antipattern. Instead, when you work on a new feature or a bugfix, make a new branch:

```
git checkout -b myFeatureBranch
```

Here, `myFeatureBranch` is a placeholder for your new branch name. Usually, it makes sense to name the branch so that you can remember the purpose of the branch. For example, if you are working on adding "scarf plots" to some analysis, naming the branch something like `scarfPlots` is a good idea. Executing `git branch` should return a list of the branches that are currently in your local repository. Executing `git checkout [branchname]` allows you to jump between different branches. Make sure to switch to the new feature branch before making any changes:

```
git checkout myFeatureBranch
```

You can also check which branch you are currently on, and what kind of state this branch is currently in, by executing

```
git status
```

To initiate the new branch also on your GitHub repository, execute

```
git push --set-upstream origin myFeatureBranch
```


Before making a new feature branch, it is usually good to switch to the `master` branch (`git checkout master`), and rebase it. That way, you will keep your `master` up to date, and make sure that your new feature branch is created clean and up to date.

#### Commit your changes

During implementing a feature, regularly commit your progress as you code. To commit changes, roughly the following workflow applies.

Check the status of your branch (what files were changed, created, deleted, etc.):

```
git status
```

If you want to see specific changes to the *code* in the files that were modified, execute `git diff [filename]` to open up an overview of what lines of code were deleted and what added. When you are done inspecting the changes, press the `Q` key on your keyboard to return to your terminal. 

Add files you want to commit:

```
git add fileToCommit1 someFolder/fileToCommit2
```

And commit your changes:

```
git commit -m "some informative message"
```

Be descriptive in your commits, and commit often (usually when you implemented some logical sub-unit of your feature/bug fix). The flag `-m` allows you to write a brief commit message. If you would like to be more verbose, you may run the command just as is (`git commit`), which opens up the configured text editor. Once you save the commit message, the commit is made. If you close the text editor without saving the commit message, the commit will not be done.

Do not forget to rebase your repository often.

#### Push your changes

Do not forget to bring your GitHub repository up to date with your local repository by pushing your commits to GitHub:

```
git push
```

Remember, GitHub serves you as a back up. If you commit your changes but do not push them to GitHub, your work may be lost if your laptop gets stolen, or your house burns down and your compurer with all your hard drive back ups with it!


#### Squashing commits (optional)

Squashing commits is usually not necessary, but can come in handy in certain situations. For example, if you work on some feature branch and make multiple commits and at some point get merge conflicts when trying to rebase. In this situation, you need to resolve the conflicts, which is done one commit at a time. So if you made a lot of commits that each had some merge conflicts with `jasp-stats`, you may end up doing a lot of work fixing the conflicts for each commit. Instead, you may squash your commits, which is basically combining multiple commits into one, and then resolve your conflicts only once.

There are multiple ways how to squash your commits. One way is to simply undo your last commits and commiting them again as one commit. An example:

```
git reset --soft HEAD~9
git commit -m "a new commit message outlining the changes in the previous 9 commits"
```

The first command `reset` is used to "undo" your commits. The option `--soft` makes sure that while you undo the commits, you *keep* the changes. All your edits are still preserved, just not commited anymore. The last argument `HEAD~9` is about how many commits you want to undo. `HEAD` points to the current status of the repository, and `HEAD~9` means "the most recent nine commits".

While this way to squash commits is easy, it has its downside. The original commit messages of the commits that you undo are undone as well, and forever lost. In case you want to combine the individual messages into one message as well, you will need to use methors like `git rebase -i <last-commit-to-keep>`, or `git merge --squash`. These methods are more advanced, so make sure you understand what you are doing. For example by studying the responses in the following thread: [https://stackoverflow.com/questions/5189560/squash-my-last-x-commits-together-using-git](https://stackoverflow.com/questions/5189560/squash-my-last-x-commits-together-using-git).

If you already pushed your commits to your GitHub repository, and then squash your commits, your local repository has diverged from the GitHub repository. To push your squashed commits, you will need to `git push -f` or `git push -force` to override the GitHub repository with the new version on your computer.




### Pull requests [(GitHub documentation)](https://docs.github.com/en/pull-requests/collaborating-with-pull-requests/proposing-changes-to-your-work-with-pull-requests/creating-a-pull-request-from-a-fork)

Pull request (PR) is a way how to review and merge your changes into the `jasp-stats` repository in a controlled and structured way, so that we can be always sure that the code on `jasp-stats` is working. 

#### Prefer smaller PRs 

Usually, a PR should implement a logically coherent feature or a bug fix, and should not bring together many unrelated changes. For example, if your task is to fix two unrelated bugs in the same module, it makes sense to split the two fixes in two different PRs. If the bugs are related (for example, you cannot fix one without fixing the other), then you make one PR for both. There are exceptions, of course, such as when the changes are so trivial and small that it is easy to keep track of everything at once, or if your PR introduces refactoring or adding a large part of the code, which happens to also fix some bugs and add multiple more features. But in general, making rather smaller PRs helps to get the PR reviewed and merged quicker and is less error prone. Large PRs tend to go stale, get nasty merge conflicts, and are difficult to review and test properly.

#### Create a pull request

After commiting your changes and pushing them to your repository, you can make a PR. Navigate your browser to your repository (`https://github.com/username/jaspRegression`) and click `Pull request` button.

1. Verify that:
  - base repository is set to: `jasp-stats/jaspRegression`, base: `master`
  - head repository is set to: `usename/jaspRegression`, compare: `myFeatureBranch`
2. Do not forget to provide a meaningful title of the PR, and a description giving additional information.
3. Use keywords such as `fixes` to link existing issues to the PR [(GitHub documentation)](https://docs.github.com/en/issues/tracking-your-work-with-issues/linking-a-pull-request-to-an-issue).

#### Assign reviewers

A PR needs to be reviewed and approved by a JASP Team member to be merged into `jasp-stats`. To signal that you consider your PR ready for a review, assign reviewers by selecting team members in the list under heading `Reviewers`. To select the appropriate reviewer, follow these rules:

1. Assign a maintainer of the module/analysis that you are proposing to change with your PR. See below for table of current maintainers.
2. If you are a trainee/student and have a mentor from the JASP Team, assign your mentor.
3. If your PR fixes an issue or a bug posted in `jasp-stats/jasp-test-release`, or `jasp-stats/INTERNAL-jasp`, you can assign the author of that issue for a review. Testers who found a bug know how to reproduce the bug, and so they are suitable to check whether the bug has been fixed. 
4. If you are not sure, or you are a maintainer of the module/analysis that you want to change with your PR, assign someone from [senior collaborators](https://github.com/orgs/jasp-stats/teams) or from [the core team](https://github.com/orgs/jasp-stats/teams/core).

<details>
	<summary>Table of module maintainers</summary>
	
| Module      | Analysis    | Maintainer |
| ----------- | ----------- | ---------- |
|jaspAnova|Classical analyses|Johnny|
|jaspAnova|Bayesian analyses|Don|
|jaspAudit|All|Koen|
|jaspBain|All|Koen|
|jaspCircular|All|Simon|
|jaspCochrane|All|Frantisek|
|jaspDescriptives|All|No one/any JASP Team member|
|jaspDistributions|All|Simon|
|jaspEquivalenceTests|All|Jill|
|jaspFactor|All|Don & Simon & Julius|
|jaspFrequencies|All|Tim & Frantisek & Simon|
|jaspJags|All|Don & Jiashun|
|jaspLearnBayes|Binary classification|Simon|
|jaspLearnBayes|Counts|Frantisek|
|jaspLearnBayes|The problem of points|Jiashun|
|jaspLearnBayes|Buffon's needle|Jiashun|
|jaspMachineLearning|All|Koen & Don|
|jaspMetaAnalysis|Bayesian and classical meta-analysis|Sophie|
|jaspMetaAnalysis|All|Frantisek|
|jaspMixedModels|All|Frantisek|
|jaspNetwork|All|Don|
|jaspRegression|Correlation|Simon|
|jaspRegression|Classical linear and classical logistic regression|Qixiang & Simon & Don|
|jaspRegression|Bayesian correlation|Alexander & Don & Simon|
|jaspRegression|Bayesian linear regression|Don|
|jaspReliability|All|Julius|
|jaspSem|All|Simon & Julius|
|jaspSummaryStatistics|All|EJ & Akash|
|jaspTTests|All|Don|
|jaspVisualModeling|All|Dustin|
|jaspProphet|All|Malte & Simon|
|jaspProcessControl|All|SKF Team|

</details>

#### GitHub Actions

After you created the PR, GitHub actions will be executed, which check the code in your PR. Most notably, the unit tests are run on instances of Windows, Mac, and Linux machine, so that we check that the PR will not break something if it is being merged. 

If any of the tests fail, the PR cannot be merged. You as an author of the PR is responsible to fix the issues with unit tests. A good idea is to run the tests on your computer (using `jaspTools::testAll()`) to check that the tests are not broken (and fix any problems) even before making a new PR. Successful checks are shown in the PR as this:

![Screenshot of successful tests](/Docs/development/img/git-guide/successful-checks.png)

If any of the tests fail, they will be shown with a red cross. Click on `details` to view the log of the test to see what went wrong. If you are unsure what you are seeing, or do not know how to fix the problem, ask for guidance someone from the JASP Team, or comment in the PR on GitHub.

#### Respond to reviews

Reviewers may require some changes to the PR. Resolve the suggestions, commit the changes, and push to your GitHub repository. 

In case that you do not receive a review within a couple of days, ask the assigned reviewer whether they are aware that they were assigned or whether they have time to review. In case they decline, assign someone else. Assigned reviewers are responsible to review your work or notify you if they cannot do so, but you are responsible for the PR as a whole.

If you think the PR is urgent, write to the collective communication channels (e.g., Slack), or get in touch with a senior JASP Team member.

#### Delete your feature branch

After your PR was reviewed, approved and merged, you may delete the feature branch that you created.

To delete the branch locally on your computer, execute:

```
git branch -d myFeatureBranch
```

To delete the branch on your GitHub repository, execute:

```
git push origin --delete myFeatureBranch
```

Do not delete it if you for whatever reason want to keep the branch!

#### Reviewing PRs [(GitHub documentation)](https://docs.github.com/en/pull-requests/collaborating-with-pull-requests/reviewing-changes-in-pull-requests/about-pull-request-reviews)

In case you have been assigned for a review, you may need to check out the code that is implemented in the PR, and try it out on your computer. To do this, you can add the author's GitHub repository as additional remote, and temporarily switch to their feature branch for testing. Assuming that the author's GitHub handle is `AnotherAuthor`, execute

```
git add remote AnotherAuthor https://github.com/AnotherAuthor/jaspRegression.git
```

which adds `AnotherAuthor` to your list of remotes in the jaspRegression repository (verify with `git remote -v`). Fetch their code:

```
git fetch AnotherAuthor
```

and temporarily checkout their feature branch:

```
git checkout AnotherAuthor/theirFeatureBranch
```

When you are done with the testing, switch to one of your branches as normally (e.g., `git checkout master`). 

In case you have been assigned for a review, but do not have time or knowledge to do so, notify the PR's author, one of the senior JASP Team members, or simply write a comment in the PR. You may suggest other contributors you think are more suitable for a review.


## Summary of git commands

Let's recap the git commands that were used in this guide, in a succession, during one cycle of a development workflow.

### Set up the repository

Clone your repo and add the `jasp-stats` remote, check that remotes are set up correctly:

```
git clone https://github.com/username/jaspRegression.git
git remote add upstream https://github.com/jasp-stats/jaspRegression.git
git remote -v
```

This needs to be done once per your local repository.

### Rebase

```
git stash
git fetch upstream
git rebase upstream/master
git push
git stash pop
```

Do this frequently.

### Make changes

Make a feature branch on your computer and on your GitHub repository. This needs to be done once for each feature branch:

```
git checkout -b myFeatureBranch
git push --set-upstream origin myFeatureBranch
```

Make your changes and commit them. Typical example of succession of commands during this phase would be something alike

```
git status
git add file1 folder1/ folder2/file2 ...
git commit -m "my commit message" 
git push
```

Rebase again before making a PR. 

You may need to make more commits and push them during the PR review.

### After PR

After the PR was approved and merged to `jasp-stats`, delete the feature branch from your computer and from your GitHub repository:

```
git delete -b myFeatureBranch
git push origin --delete myFeatureBranch
```

This needs to be done once for each feature branch. Do not delete it if you want to keep the branch!

## Git submodules: `jasp-stats/jasp-desktop`

As mentioned previously, JASP modules are separate repositories on GitHub. There is one repository, `jasp-stats/jasp-desktop` ([https://github.com/jasp-stats/jasp-desktop](https://github.com/jasp-stats/jasp-desktop)), that has all the code required to build the JASP application. Usually, you do not need to do anything with this repository, and should work on the separate repositories. You need `jasp-desktop` only if you want to build JASP ([Guide to Building JASP](https://github.com/jasp-stats/jasp-desktop/blob/development/Docs/development/jasp-building-guide.md)) or if you want to make changes to the core JASP code and not to the code associated with a specific analysis. 

All other repositories are included in `jasp-desktop` as "git submodules". This makes `jasp-desktop` a little bit special because it requires some more steps to keep the submodules up to date. You can rebase as normally, but that will not rebase the individual submodules. Instead, run

```
git fetch --all
git submodule init
git submodule update --remote
```

`git fetch --all` fetches all remotes and submodules, `git submodule init` initiates all submodules in your local repo that were created, and `git submodule update --remote` updates local submodules to be up to date with the current version of the `master` branch of the submodules on `jasp-stats`. This means that whenever a PR to one of the modules has been accepted the commits are added to that branch and everyone who builds their own copy of JASP then gets this updated code once they run git submodule update --remote. In a nutshell it means that it'll keep all your modules up to date, which the command itself of course already seemed to imply quite strongly.

### Editing in submodules

Be careful with editing in the submodules though! Because these are copies of the `jasp-stats` based repositories it is unlikely you have push-rights and even if you do you should go through the proper channels, that is, PR + review.

### Important branches

All jaspModules (e.g., `jaspRegression`) on `jasp-stats` have a `master` branch, which is the default branch. This is not the case for `jasp-stats/jasp-desktop`. This repository has two important branches, `stable` and `development`. The `stable` branch, as its name suggest, is a version of `jasp-desktop` that is stable, usually very similar to the version that was in the latest release of JASP. This version should be always ready to be build in Qt without problems, and should just work. However, it may not contain the latest hot changes to the JASP application. For that version, there is the branch `development`, which containts the current version of the JASP application being developed for the next release. `development` is the default branch of `jasp-desktop`. **Be careful about which branch you use when you rebase**, i.e., `git rebase upstream/stable` or `git rebase upstream/development`. 

## Summary of basic principles


1. Do not use git clients for making PRs, unless you know what you are doing.
2. Learn working with git comfortably.
3. Fork and clone jaspModules to make PRs, do not work inside of `jasp-desktop` repository.
4. Work in feature branches not on `master`.
5. Rebase often. **Do not** call `git pull` to sync your repository with `jasp-stats`. **Rebase**.
6. Push your changes regularly from your local repository to your GitHub repository, to back up your work in progress.
7. Prefer making more smaller PRs rather than making one PR implementing many unrelated features.
8. Make clean PRs
	- Make sure you do not have merge conflicts (rebase)
	- Test your code before making a PR
	- Document your changes by writing informative commit messages
	- Link issues to your PR
9. You are responsible for making sure your PR will be dealt with. Assign reviewers to your PRs. Communicate when your PR goes stale or if your PR needs special attention.
10. Be proactive when assigned to review: let the PR author know if you cannot review their PR.
11. You can add other people's forks as your remote to review or checkout their code.

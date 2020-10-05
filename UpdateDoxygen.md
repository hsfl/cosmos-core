# Updating the Doxygen page https://hsfl.github.io/cosmos-core 

## 1. Create an Account on [Travis CI](https://travis-ci.org/)
Link your github account and turn on Travis for your repository in question, using the Travis control panel. 

If the Hawaii Space Flight organization does not show, go to Settings. On the left side of the window under **Organizations**, click `Review and add your authorized organizations` and add the organization. 

## 2. Provide Travis CI with encrypted credentials for publishing to `gh-pages`
### Generating a personal access token for GitHub
1. In GitHub, go to Settings -> Developer Settings -> Personal Access tokens
2. Click `Generate a new token` and fill in the description. E.g. `<your_repo> Travis CI Documentation`
3. As scope select `public_repo`
4. Click `Generate token`
5. Copy the generated token 

### Adding the secure variable to Travis CI
1. Go to the repository in Travis CI and go to settings
2. In the Environment Variables section, give the variable a `Name`, in this case `GH_REPO_TOKEN`
3. Paste the copied token as `Value`
4. Click on `Add`
5. Make sure the `Display value in build log` switch is `OFF`.

## 3. Trigger Build
1. Go to the repository in Travis CI and go Build History 
2. Click on the branch and Trigger Build

I found the instructions [here](https://gist.github.com/vidavidorra/548ffbcdae99d752da02) and slightly modified. 

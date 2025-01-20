+++
date = '2025-01-20T18:34:20+03:00'
draft = false
title = 'Host Your Blog'
+++

## Website Hosting

We have successfully created a new website. That required some work, but it still was quite fast and easy (at least in my opinion). We have a problem, though. Nobody can see your website, other than you. When you type:

```bash
hugo server
```

You can see what is called a *local* version of your website. It means that it is accessible only on your computer (and maybe on your local network if you know your computer IP address). But the whole point of the website is to be visible to other people on the internet. What is the point of your website if nobody can see it?

How can we solve this problem? From the technical perspective, we need to do three things.

Firstly, we need to obtain a computer with a globally accessible IP address. This computer has to be available 24/7. Your computer probably does not have a global IP address (maybe your ISP can sell it to you for additional cost), and you are not using it 24/7. Most people temporarily rent a remote computer (or even just a part of the resources of a computer) called *server*.

After that, we need to obtain a globally accessible IP address. It means that anyone around the world should be able to access your computer from the internet using this IP address. This address is usually provided by a hosting company.

The last thing you need is a nice address, like `www.mywebsite.com`. People never (almost) use something like `42.86.104.28` to access a website. We are not computers and prefer to use words instead of numbers. You have to obtain a *domain address*, which is usually sold for money by domain providers. After that, you need to configure a DNS, such that your domain points to the IP address of your server.

As you can see, this process is quite nuanced. This is the reason why many companies take care of most of these things. All you have to do is click a couple of buttons, and your website is ready. This costs money, and you need to pay for this each month.

## Github Pages

We are not doing any rocket science here, and our blog will be primarily textual. Do we need to go through all of those steps and pay for hosting each month?

Luckily for us, some solutions provide free hosting for your websites as long as your websites are *static*, which means that they consist of formatted text and files, and do not contain any computational logic like in web applications (think about blogs or e-commerce websites).

The most popular solution is [Github Pages](https://pages.github.com/). I am going to cover how to host our website on this platform. You can also read details [here](https://gohugo.io/hosting-and-deployment/hosting-on-github/).

## Github Repository

Before we start, you have to create an account on the [github](https://github.com). This is required for GitHub pages but is also useful for GitHub to host your source code repositories.

After you have created your account and logged in, [create a new repository](https://github.com/new). Call it whatever you like.

Now you need to "push" your repository to GitHub. Before we do that, make sure that your repository contains all of your files. Add all files into staging and make a commit:

```bash
git add .
git commit -m 'first commit'
```

If your git installation is fresh, you may need to configure your git. Follow the instructions on your terminal. Feel free to change the commit message to whatever you like.

Let's add a remote repository to our GitHub repo:

```bash
git remote add origin https://github.com/username/repository
```

Change `username` and `repository` to your GitHub username and repository you created. Now let's rename our current branch to `main`:

```bash
git branch -m main
```

Finally, let's push our local repository to GitHub:

```bash
git push -u origin main
```

## Github Action

Now that our website is on GitHub, let's make some configurations. Visit your GitHub repository. From the main menu choose Settings->Pages. Change `Source` to `Github Actions`.

Create a new file `hugo.yaml` in a directory named `.github/workflows`:

```bash
mkdir -p .github/workflows
touch hugo.yaml
```

Now paste this into your `hugo.yaml` file:

```yaml
# Sample workflow for building and deploying a Hugo site to GitHub Pages
name: Deploy Hugo site to Pages

on:
  # Runs on pushes targeting the default branch
  push:
    branches:
 - main

  # Allows you to run this workflow manually from the Actions tab
  workflow_dispatch:

# Sets permissions of the GITHUB_TOKEN to allow deployment to GitHub Pages
permissions:
  contents: read
  pages: write
  id-token: write

# Allow only one concurrent deployment, skipping runs queued between the run in progress and the latest queued.
# However, do NOT cancel in-progress runs as we want to allow these production deployments to complete.
concurrency:
  group: "pages"
  cancel-in-progress: false

# Default to bash
defaults:
  run:
    shell: bash

jobs:
  # Build job
  build:
    runs-on: ubuntu-latest
    env:
      HUGO_VERSION: 0.141.0
    steps:
 - name: Install Hugo CLI
        run: |
 wget -O ${{ runner.temp }}/hugo.deb https://github.com/gohugoio/hugo/releases/download/v${HUGO_VERSION}/hugo_extended_${HUGO_VERSION}_linux-amd64.deb \
 && sudo dpkg -i ${{ runner.temp }}/hugo.deb
 - name: Install Dart Sass
        run: sudo snap install dart-sass
 - name: Checkout
        uses: actions/checkout@v4
        with:
          submodules: recursive
          fetch-depth: 0
 - name: Setup Pages
        id: pages
        uses: actions/configure-pages@v5
 - name: Install Node.js dependencies
        run: "[[ -f package-lock.json || -f npm-shrinkwrap.json ]] && npm ci || true"
 - name: Build with Hugo
        env:
          HUGO_CACHEDIR: ${{ runner.temp }}/hugo_cache
          HUGO_ENVIRONMENT: production
          TZ: America/Los_Angeles
        run: |
 hugo \
 --gc \
 --minify \
 --baseURL "${{ steps.pages.outputs.base_url }}/"
 - name: Upload artifact
        uses: actions/upload-pages-artifact@v3
        with:
          path: ./public

  # Deployment job
  deploy:
    environment:
      name: github-pages
      url: ${{ steps.deployment.outputs.page_url }}
    runs-on: ubuntu-latest
    needs: build
    steps:
 - name: Deploy to GitHub Pages
        id: deployment
        uses: actions/deploy-pages@v4
```

Finally, commit and push the change to your GitHub repository:

```bash
git add -A
git commit -m "create hugo.yaml"
git push
```

Now go to your GitHub repository. Go to `Actions`. After the build is finished, click on the commit message. You will see the link to your website. Click here to visit your website.

Congratulations, you are online.
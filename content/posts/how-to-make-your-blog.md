+++
date = '2025-01-20T16:06:51+03:00'
draft = false
title = 'How to Make Your Blog'
+++

### Website constructors

Do you want to create your blog? There are many possible ways to do that. The most popular and accessible ways are targeted for broad and general audiences. Here are some useful links:

- [WordPress](https://wordpress.com) is probably the oldest and the most popular website constructor. It has wide customization options.
- [Wix](https://wix.com) is a fresh solution targeted at a niche audience.
- [Medium](https://medium.com) is another popular option. I've seen many blogs on this platform that discuss more business or technical topics.

### Other Options

Of course, there some many other possible options and platforms that could help you create your blog. But I want to talk about some other options. What if you don't want to use existing ready platforms, and want to try and create something else?

I want to warn you that the methods that I am going to discuss are more technical and require more prerequisite knowledge. At the very least, you should know how to use terminal and git. Another method that I am going to discuss later will require even more knowledge in advance.

There are two ways to create your blog. Quick and easy, and long and more complex. Don't hurry to discard the second option, though, because it has its use cases as we will discuss later.

### Text Markup

Before we dive into details, let us digress a little to discuss text formatting. If you read any sort of blog, you will probably notice that text looks different depending on its meaning. The heading can be big and placed in the middle, some text can be *italicized* for slight emphasis, or **bold** for strong emphasis. There can be some [links]() leading to other pages, or images.

There are many different ways to specify all of those details and features, called *markup*. For example, Microsoft Office uses .docx files to specify markup text, there are also some markup features supported in email. But we are interested in a *web browser*, software that you use to read this page right now.

Web browsers use a special file format called *HTML* for markup. It looks somewhat like this:

```html
<!DOCTYPE html>
<html>
    <head>
        <title>My Page</title>
        <meta charset="utf-8">
        <style type="text/css"></style>
    </head>
    <body>
        <div>
            <h1>Page Title</h1>
            <p>This is a parargaph.</p>
        </div>
    </body>
</html>
```

You can see that it contains quite a few details like angle brackets, slashes, equal signs, and quotes. Notice that this file format has strict formatting rules. If you place an element in the wrong place or forget to close your tag, your file will be invalid, and the browser won't display it properly. You can see that this file format can be erroneous, but also quite tedious to write by hand.

This is the reason why a new file format was created, called *markdown*. It is much easier to both read and write, and it's not as strict. Take a look at what it looks like:

```markdown
### Page Title

This is a paragraph.

- first item
- second item
- third item

Here is a [link](https://example.com) to a website.

This is *italic* text and **bold** text.
```

This is what this text looks like:

### Page Title

This is a paragraph.

- first item
- second item
- third item

Here is a [link](https://example.com) to a website.

This is *italic* text and **bold** text.

If you are curious about markdown format, [here](https://commonmark.org/help/) is a brief but very helpful reference.

### Markup Framework

As you can see, this is much easier to use compared to HTML. There is a problem, though. As you recall, browsers only support HTML files. They don't support the markdown format. This means that we need to use some software that converts markdown to html (and also applies themes, so that we don't have to write them ourselves).

Many different projects aim to achieve that. I personally like [hugo](https:/gohugo.io). It's fast, modern, and easy to use. I'm using it to build the blog you are reading right now. I'll walk you through details on how to use it, but you can also read details [here](https://gohugo.io/getting-started/quick-start/).

### Hugo Tutorial

Before we proceed, some software should be installed on your computer:

- [Hugo](https://gohugo.io)
- [Git](https://git-scm.com)

After you install it, open your terminal and navigate to the directory where you want to place your Hugo project. After that, type those commands:

```bash
hugo new site quickstart
cd quickstart
git init
git submodule add https://github.com/theNewDynamic/gohugo-theme-ananke.git themes/ananke
echo "theme = 'ananke'" >> hugo.toml
hugo server
```

The first line creates a new directory with the website (feel free to change `quickstart` to whatever you like). The second line changes the active directory to a website directory. The third line initializes a git repository. The next line adds the `ananke` theme to your website. Line after that changes the website configuration to apply this theme. The last line creates a local instance of your website.

Let's add a new page to our website. Type this:

```bash
hugo new content content/posts/my-first-post.md
```

Feel free to change the title of your page to whatever you want. Your page is located in the `content/posts` directory. It looks something like this:

```markdown
+++
title = 'My First Post'
date = 2024-01-14T07:07:07+01:00
draft = true
+++
```

Notice a `draft` field. Pages marked as drafts can be built using a special flag, but they won't be visible on the published site. After you are done editing your page, don't forget to change the value of this field to `false`.

Add some markdown to your page:

```markdown
### Introduction

This is **bold** text, and this is *emphasized* text.

Visit the [Hugo](https://gohugo.io) website!
```

Finally, build your site:

```bash
hugo server -D
```

You can see that your page is now visible.

Visit the Hugo site. You can find many helpful resources, like site themes or documentation here.

### The Long Way

You can probably remember that I mentioned in the beginning that there is a second way that you can use to create your website. Well, it is, of course, longer and more time-consuming, but allows much higher flexibility and customization. In a nutshell, you write your code using HTML, CSS, JavaScript, and other languages. This takes a much longer time but allows you to have much more control over what is going on. I am not going to go into this rabbit hole now, but there are many useful resources to learn about it. For example, [MDN](https://developer.mozilla.org/en-US/) is a canonical reference for web development.

Good luck on your journey and have fun.
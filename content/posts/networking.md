+++
title = 'Networking'
date = 2024-10-01T21:58:44+03:00
draft = false
+++

I want to talk about networking. Not about connections with other people though. Both because I am terrible 
at this, and because I have nothing to say about it. I want to talk about network as an interconnected 
set of computing devices. It is probably nice to have a general idea about how network works, at least at the 
conceptual level. Some tasks can be related to networking, be it online games or blockchain wallets. 
There are, of course, many other applications of that as well. I do not think that it is good to delve too far 
into that topic, unless you are genuinely interested in that. One possible project could be to implement a 
simple HTTP server from TCP protocol abstractions. HTTP requests are fairly straightforward:

```
GET / HTTP/1.1
Host: example.com
User-Agent: curl/8.6.0
Accept: */*
```

Responses are fairly simple as well:

```
HTTP/1.1 200 OK
Content-Type: text/html; charset=UTF-8
Date: Mon 1 Jan 2000 12:00:00 GMT
Content-Length: 123

<!doctype html>
...
```

All you have to do is to create a TCP connection, listen to it and then parse the text of the incoming 
request. There is actually a [great place](https://app.codecrafters.io/courses/http-server/overview) to try 
and build your own. It is fairly simple.

Networking is complex and multilayered. As I said earlier, it is good to have a coneptual grasp of what is 
going on, but I don't think that you have to understand all the fine-grained details, especially on the lowest 
levels, since they are more os and hardware specific. 
There is also a [resource](https://developer.mozilla.org/en-US/docs/Learn/Common_questions/Web_mechanics) 
on MDN considering 'web mechanics'.

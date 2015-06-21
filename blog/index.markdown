---
author: admin
comments: true
date: 2008-08-18 20:10:40+00:00
layout: page
slug: blog
title: Blog
wordpress_id: 118
---

<div>

  <ul>
    {% for post in site.posts %}
      <li><a class="post-link" href="{{ site.baseurl }}/{{ post.url }}">{{ post.date | date: "%b %-d, %Y" }} : {{ post.title }}</a> </li>
    {% endfor %}
  </ul>

  <p>subscribe <a href="{{ site.baseurl }}/atom.xml">via RSS</a></p>

</div>   

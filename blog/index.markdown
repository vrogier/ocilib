---
author: admin
comments: true
date: 2019-02-25 20:00:00+01:00
layout: page
slug: blog
title: Blog
---

<div>

  <ul>
    {% for post in site.posts %}
      <li><a class="post-link" href="{{ site.baseurl }}/{{ post.url }}">{{ post.date | date: "%b %-d, %Y" }} : {{ post.title }}</a> </li>
    {% endfor %}
  </ul>

  <p>subscribe <a href="{{ site.baseurl }}/atom.xml">via RSS</a></p>

</div>   

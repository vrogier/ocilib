---
author: admin
comments: false
date: 2017-01-25 22:35:00+01:00
layout: post
slug: ocilib-v4-3-1-now-available-for-download
title: OCILIB v4.3.1 now available for download !
categories:
- News
- Updates
---

Dear all,
<br/>
<br/>
<b>OCILIB v4.3.1 is now [_available for download_]({{site.projecturl}}/releases/) !</b>
<br/>
<br/>
Get the release and enjoy it !
<br/>
<br/>

### Fixes (C API)
```
    - Issue #60: Fixed bug introduced in v4.3.0 related to binding arrays of big ints
    - Issue #64: Fixed structure size computation for Object members that are Objects
    - Issue #65: Fixed Binding null/non initialized OCI_Number
    - Issue #68: Fixed OCI_Execute() behavior with PL/SQL "select into" and no data found
```

### Fixes (C++ API)
```
    - Issue #63: Added cstddef header in ocilib_core.hpp as on some platforms/compiler versions, it was reported as needed
    - Issue #67: Fixed memory leak when using Number objects
```

### Changes:
```
    - Request #66: Increased value of OCI_BIND_MAX from 1024 to 65535
```

<br/>

Vincent


# Transport-Catalogue
Pet project developed during Yandex Praktikum course for study purposes
<details><summary>How to Use</summary>
<p>

#### We can hide anything, even code!
  <details><summary>Format of input data</summary>
  <p>
      
  </p>
  </details>
  <details><summary>Format of input data</summary>
  <p>
      
  </p>
  </details>  

</p>
</details>




# Transport-Catalogue
Pet project developed during Yandex Praktikum course for study purposes.
## Description
The catalogue receives JSON requests and provide map of the routes visualized with SVG\ 
The input JSON data is verified for standard compliance with built-in verificator\
Main goal of this education project is to practice application of following topics:

- Inheritance
- JSON reading and building
- Library <variant>
- Building of SVG
- Associative containers
  
Received JSON requests filling the database with information about stops(geo location, name and distance to other stops)
and buses(name, stops on the route, type bus).\
Filled catalogue can process following output requests:
  - stop request - information about buses, which are stopping at this stop
  - bus request - information about route length, number of all and unique stops and relationship between actual and geographical route length
  - map request - printing out SVG code with graphical information of all routes and stops
  
  
  
#### Below can be seen the diagram of relationships between classes and interfaces used in the program
  
![image](https://user-images.githubusercontent.com/82179428/159783017-995fd10c-171a-4c10-ad1e-4bec009307cf.png)  


## How to use

### Format of input data

  The input data should be provided as a single JSON file. There three main types of requests, provided as keys of dictionary:
  
```
  {
  "base_requests": [ ... ],
  "render_settings": { ... },
  "stat_requests": [ ... ]
  }
```
 `base_requests` - array with routes and stops info\
 `render_settings` - dictionary with visualization settings\
 `stat_requests` - array with output requests 
  
#### Filling of the database
  
  Array `base_requests` can contain requests to add route and to add stop. Important to notice that all stops declared in route should be defined.\
  Example of request to add a route:
  
  
  ```
  {
  "type": "Bus",
  "name": "411",
  "stops": [
    "Rathaus",
    "Hochstrasse",
    "Hauptbahnhof",
    "Am Wassertor",
    "Rathaus"
    ],
  "is_roundtrip": true
  }
  
  ```
  
  Description of the route is dictionary with following keys:
  `type` - defining the type of the request - "Bus" for route definition\
  `name` - name of the route\
  `stops` - array with stops, sorted according to the route\
  `is_roundtrip` - if true, then bus travels in circle, means first and last stop in array `stops` is the same; after arriving at last stop, route starts from the first one. 
  
  

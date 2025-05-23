program arrayReduction
   real, dimension(3,2) :: a 
   a = reshape( (/5,9,6,10,8,12/), (/3,2/) ) 
   
   Print *, all(a>5)
   Print *, any(a>5)
   Print *, count(a>5)
   Print *, all(a>=5 .and. a<10)
  
end program arrayReduction

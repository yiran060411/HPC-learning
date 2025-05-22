program stop_example     
implicit none
   integer :: i     
   do i = 1, 20          
   
      if (i == 5) then 
         stop          
      end if         
      
      print*, i      
   end do  
   
end program stop_example

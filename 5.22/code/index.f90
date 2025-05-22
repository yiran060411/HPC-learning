program testingChars
implicit none
   character (80) :: text 
   integer :: i 
   
   text = 'The intrinsic data type character stores characters and   strings.'
   i=index(text,'character') 
   
   if (i /= 0) then
      print *, ' The word character found at position ',i 
      print *, ' in text: ', text 
   end if
   
end program testingChars

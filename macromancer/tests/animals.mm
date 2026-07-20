$interface Animal as Dynamic {
	eat
	speak
	sleep
}

$impl Dog as Animal {
	$header = "dog_cat.h"
	speak = dog_speak
	eat = dog_eat
	sleep = dog_sleep
}

$impl Cat as Animal {
	$header = "dog_cat.h"
	eat = cat_eat
	speak = cat_speak
	sleep = cat_sleep
}

$export Animal as Dog

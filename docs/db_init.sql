CREATE TABLE public.passengers (
	passenger_id serial4 NOT NULL,
	first_name varchar(100) NOT NULL,
	last_name varchar(100) NOT NULL,
	passport_number varchar(20) NULL,
	CONSTRAINT passengers_pkey PRIMARY KEY (passenger_id)
);

CREATE TABLE public.sessions (
	id serial4 NOT NULL,
	username varchar(255) NOT NULL,
	"token" varchar(255) NOT NULL,
	expiration timestamp NOT NULL,
	CONSTRAINT sessions_pkey PRIMARY KEY (id),
	CONSTRAINT sessions_un UNIQUE (username)
);

CREATE TABLE public.users (
	id serial4 NOT NULL,
	username varchar(255) NOT NULL,
	email varchar(255) NOT NULL,
	password_hash varchar(64) NOT NULL,
	created_at timestamp NULL DEFAULT CURRENT_TIMESTAMP,
	passenger_id int4 NULL,
	balance int4 NOT NULL DEFAULT 0,
	CONSTRAINT users_email_key UNIQUE (email),
	CONSTRAINT users_pkey PRIMARY KEY (id),
	CONSTRAINT users_username_key UNIQUE (username),
	CONSTRAINT users_fk FOREIGN KEY (passenger_id) REFERENCES public.passengers(passenger_id)
);

CREATE TABLE public.bookings (
	booking_id serial4 NOT NULL,
	flight_id int4 NULL,
	passenger_id int4 NULL,
	booking_date timestamp NULL DEFAULT CURRENT_TIMESTAMP,
	seat_number varchar(10) NULL,
	CONSTRAINT bookings_pkey PRIMARY KEY (booking_id),
	CONSTRAINT bookings_flight_id_fkey FOREIGN KEY (flight_id) REFERENCES public.flights(flight_id),
	CONSTRAINT bookings_passenger_id_fkey FOREIGN KEY (passenger_id) REFERENCES public.passengers(passenger_id)
);

CREATE TABLE public.flights (
	flight_id serial4 NOT NULL,
	departure_airport_id int4 NULL,
	arrival_airport_id int4 NULL,
	plane_id int4 NULL,
	departure_time timestamp NOT NULL,
	arrival_time timestamp NOT NULL,
	price int4 NULL,
	CONSTRAINT flights_pkey PRIMARY KEY (flight_id),
	CONSTRAINT flights_arrival_airport_id_fkey FOREIGN KEY (arrival_airport_id) REFERENCES public.airports(airport_id),
	CONSTRAINT flights_departure_airport_id_fkey FOREIGN KEY (departure_airport_id) REFERENCES public.airports(airport_id),
	CONSTRAINT flights_plane_id_fkey FOREIGN KEY (plane_id) REFERENCES public.planes(plane_id)
);

CREATE TABLE public.planes (
	plane_id serial4 NOT NULL,
	model varchar(100) NOT NULL,
	capacity int4 NOT NULL,
	CONSTRAINT planes_pkey PRIMARY KEY (plane_id)
);

CREATE TABLE public.airports (
	airport_id serial4 NOT NULL,
	"name" varchar(100) NOT NULL,
	city varchar(100) NOT NULL,
	country varchar(100) NOT NULL,
	CONSTRAINT airports_pkey PRIMARY KEY (airport_id)
);

CREATE TABLE public.reserves (
	passenger_id int4 NOT NULL,
	flight_id int4 NOT NULL,
	CONSTRAINT reserves_pkey PRIMARY KEY (passenger_id, flight_id),
	CONSTRAINT reserves_flight_id_fkey FOREIGN KEY (flight_id) REFERENCES public.flights(flight_id) ON DELETE CASCADE,
	CONSTRAINT reserves_passenger_id_fkey FOREIGN KEY (passenger_id) REFERENCES public.passengers(passenger_id) ON DELETE CASCADE
);
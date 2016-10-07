drop table if exists RemainTimeTable;
create table RemainTimeTable(
UID char(10) primary key,
RemainTime int default 0
);

drop table if exists OnTable;
create table if not exists OnTable(
UID char(10) primary key,
RemainTime int default 0,
StartTime datetime,
isOvertime boolean default false,
foreign key(UID) references RemainTimeTable(UID)
);
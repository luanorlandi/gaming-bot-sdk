day = {31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31}

week = {"Monday", "Tuesday", "Wednesday","Thursday",
    "Friday", "Saturday", "Sunday"}

week["Monday"] = 1
week["Tuesday"] = 2
week["Wednesday"] = 3
week["Thursday"] = 4
week["Friday"] = 5
week["Saturday"] = 6
week["Sunday"] = 7

month = {}

month["Jan"] = 1
month["Feb"] = 2
month["Mar"] = 3
month["Apr"] = 4
month["May"] = 5
month["Jun"] = 6
month["Jul"] = 7
month["Aug"] = 8
month["Sep"] = 9
month["Oct"] = 10
month["Nov"] = 11
month["Dec"] = 12

function countDays(sM, sD, tM, tD)
    if sM == tM then
        return math.abs(sD - tD)
    end

    if sM > tM then
        sM, tM = tM, sM
        sD, tD = tD, sD
    end

    local days = tD + (day[sM] - sD)

    for i = sM+1, tM-1 do
        days = days + day[i]
    end

    return days
end

local leapYear = tonumber(io.read())
local next_token = string.gmatch(io.read(), "[^%s]+")
local sourceDayOfWeek = next_token()
local sourceMonth = next_token()
local sourceDayOfMonth = tonumber(next_token())
local next_token = string.gmatch(io.read(), "[^%s]+")
local targetMonth = next_token()
local targetDayOfMonth = tonumber(next_token())

if leapYear == 1 then
    day[2] = 29
end

local days = countDays(month[sourceMonth], sourceDayOfMonth,
    month[targetMonth], targetDayOfMonth)

if month[sourceMonth] > month[targetMonth] or
    (month[sourceMonth] == month[targetMonth] and
    sourceDayOfMonth > targetDayOfMonth) then

    days = -days
else
    days = days
end

days = (week[sourceDayOfWeek] + days) % 7

if days == 0 then
    days = 7
end

print(week[days])